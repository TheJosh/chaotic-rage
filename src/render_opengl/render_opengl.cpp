// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <algorithm>
#include <map>
#include <sstream>
#include "gl.h"
#include <SDL_image.h>
#include <math.h>

#include "../physics/physics_bullet.h"
#include "../game_state.h"
#include "../game_engine.h"
#include "../game_settings.h"
#include "../entity/unit/player.h"
#include "../entity/vehicle.h"
#include "../util/clientconfig.h"
#include "../util/obj.h"
#include "../util/sdl_util.h"
#include "../util/windowicon.h"
#include "../mod/mod_manager.h"
#include "../mod/vehicletype.h"
#include "../mod/weapontype.h"
#include "../map/map.h"
#include "../map/heightmap.h"
#include "../map/mesh.h"
#include "../render/sprite.h"

#include "render_opengl.h"
#include "render_opengl_settings.h"
#include "gl_debug.h"
#include "gl_debug_drawer.h"
#include "glvao.h"
#include "glshader.h"
#include "assimpmodel.h"
#include "animplay.h"
#include "light.h"
#include "hud.h"
#include "renderer_heightmap.h"

#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <guichan/opengl.hpp>
#include "guichan_imageloader.h"
#include "opengl_font.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../spark/SPK.h"

#define BUFFER_MAX 200

using namespace std;


/**
* For VBO pointer offsets
**/
#define BUFFER_OFFSET(i) ((char *)NULL + (i))


/**
* Used for shadows
**/
glm::mat4 biasMatrix(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
);


/**
* Just set a few things up
**/
RenderOpenGL::RenderOpenGL(GameState* st, RenderOpenGLSettings* settings) : Render3D(st)
{
	SDL_InitSubSystem(SDL_INIT_VIDEO);

	// TODO: Window icon for SDL2
	/*SDL_RWops *rw = SDL_RWFromConstMem(windowicon_bmp, sizeof(windowicon_bmp));
	SDL_Surface *icon = SDL_LoadBMP_RW(rw, 1);
	SDL_SetColorKey(icon, SDL_SRCCOLORKEY, SDL_MapRGBA(icon->format, 255, 0, 255, 0));
	SDL_WM_SetIcon(icon, NULL);
	SDL_FreeSurface(icon);*/

	this->window = NULL;
	this->glcontext = NULL;
	this->physicsdebug = NULL;
	this->speeddebug = false;
	this->viewmode = GameSettings::behindPlayer;
	this->render_player = NULL;
	this->render_player_pos = glm::vec3(0.0f);

	this->sprite_vbo = 0;

	this->skybox_vao = NULL;
	this->gui_font = NULL;

	this->settings = NULL;
	this->setSettings(new RenderOpenGLSettings());

	this->shaders_loaded = false;
	this->ambient_daynight = NULL;
}


/**
* Cleanup
**/
RenderOpenGL::~RenderOpenGL()
{
	delete this->settings;
	delete this->gui_font;

	std::map<std::string, OpenGLFont*>::iterator it;
	for (it = this->fonts.begin(); it != this->fonts.end(); ++it) {
		delete it->second;
	}
	this->fonts.clear();
	
	// TODO: Delete all buffers, tex, etc.

	#ifndef SDL1_VIDEO
		SDL_GL_DeleteContext(this->glcontext);
	#endif
}



/**
* Set the settings to use
**/
void RenderOpenGL::setSettings(RenderOpenGLSettings* settings)
{
	delete(this->settings);
	this->settings = settings;

	if (!GL_EXT_texture_filter_anisotropic) {
		this->settings->tex_aniso = 0.0f;
	}

	// Texture filtering modes
	switch (this->settings->tex_filter) {
		case 4:
			// trilinear
			this->min_filter = GL_LINEAR_MIPMAP_LINEAR;
			this->mag_filter = GL_LINEAR;
			break;

		case 3:
			// bilinear
			this->min_filter = GL_LINEAR_MIPMAP_NEAREST;
			this->mag_filter = GL_LINEAR;
			break;

		case 2:
			// bilinear
			this->min_filter = GL_LINEAR_MIPMAP_NEAREST;
			this->mag_filter = GL_NEAREST;
			break;

		default:
			// crappy
			this->min_filter = GL_NEAREST_MIPMAP_NEAREST;
			this->mag_filter = GL_NEAREST;
			break;
	}

	// Update tex with new filtering modes
	for (unsigned int i = 0; i != loaded.size(); i++) {
		glBindTexture(GL_TEXTURE_2D, loaded[i]->pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->min_filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->mag_filter);
		if (this->settings->tex_aniso >= 1.0f) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, this->settings->tex_aniso);
		}
	}
}


/**
* Return a pointer to the settings object
**/
RenderOpenGLSettings* RenderOpenGL::getSettings()
{
	return this->settings;
}


/**
* Sets the screen size
* Also sets stuff up, check version, init libraries, etc
**/
void RenderOpenGL::setScreenSize(int width, int height, bool fullscreen)
{
	// Fullscreen support
	#ifdef SDL1_VIDEO
		int flagsSDL = SDL_OPENGL;
		if (fullscreen) flags |= SDL_FULLSCREEN;
	#else
		int flagsSDL = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
		if (fullscreen) {
			// Set the resolution to same as the desktop
			SDL_DisplayMode mode;
			int result = SDL_GetDesktopDisplayMode(0, &mode);
			if (result != 0) {
				reportFatalError("Unable to determine current display mode");
			}
			width = mode.w;
			height = mode.h;
			GEng()->cconf->width = width;
			GEng()->cconf->height = height;
			flagsSDL |= SDL_WINDOW_FULLSCREEN;
		}
	#endif

	this->real_width = width;
	this->real_height = height;

	// If we're on a high-res device, make the GUIs larger
	if (this->real_width > 3000) {
		GEng()->gui_scale = 2.0f;
	}

	// Window title
	char title[BUFFER_MAX];
	snprintf(title, BUFFER_MAX, "Chaotic Rage %s", VERSION);

	// Create window
	#ifdef SDL1_VIDEO
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		// In SDL1, the context is created at the same time
		if (this->settings->msaa >= 2) {
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, this->settings->msaa);
		} else {
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
		}

		this->window = SDL_SetVideoMode(width, height, 32, flags);
	#else
		if (this->window == NULL) {
			this->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flagsSDL);
		} else {
			SDL_SetWindowSize(this->window, width, height);
			if (fullscreen) {
				flagsSDL = SDL_WINDOW_FULLSCREEN;
			} else {
				flagsSDL = 0;
			}
			SDL_SetWindowFullscreen(this->window, flagsSDL);
		}
	#endif
	
	// Did it work?
	if (this->window == NULL) {
		char buffer[BUFFER_MAX];
		snprintf(buffer, BUFFER_MAX, "Unable to set %ix%i video: %s\n", width, height, SDL_GetError());
		reportFatalError(buffer);
	}

	// Create OpenGL context
	#ifndef SDL1_VIDEO
		if (this->settings->msaa >= 2) {
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, this->settings->msaa);
		} else {
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
		}

		// We might do this later on; code should be compatible
		//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		// GL context creation
		if (this->glcontext == NULL) {
			this->glcontext = SDL_GL_CreateContext(this->window);
			if (this->glcontext == NULL) {
				reportFatalError("Unable to create GL context");
			}
		}
	#endif
	
	// SDL_Image
	int flagsIMG = IMG_INIT_PNG;
	int initted = IMG_Init(flagsIMG);
	if ((initted & flagsIMG) != flagsIMG) {
		reportFatalError("Failed to init required png support.");
	}

	// Check OpenGL version
	if (atof((char*) glGetString(GL_VERSION)) < 3.0) {
		reportFatalError("OpenGL 3.0 or later is required, but not supported on this system");
	}

	// Init GLEW
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		GL_LOG("Glew Error: %s", glewGetErrorString(err));
		reportFatalError("Unable to init the library GLEW.");
	}

	// Some versions of GLEW in experimental mode throw an OpenGL error
	// so just gobble it up.
	glGetError();
	
	// GL_ARB_framebuffer_object -> shadows and glGenerateMipmap
	if (! GL_ARB_framebuffer_object) {
		reportFatalError("OpenGL 3.0 or the extension 'GL_ARB_framebuffer_object' not available.");
	}
	
	// GL_ARB_instanced_arrays -> instancing
	if (! GL_ARB_instanced_arrays) {
		reportFatalError("OpenGL 3.3 or the extension 'GL_ARB_instanced_arrays' not available.");
	}

	// Windows only: Re-load textures.
	// All other platforms don't destory the context if the window size changes
	#ifdef _WIN32
		for (unsigned int i = 0; i != loaded.size(); i++) {
			this->surfaceToOpenGL(loaded.at(i));
		}
	#endif

	// (re-)load shaders
	// If the base mod isn't yet loaded, will only load the "basic" shader.
	this->loadShaders();
	if (this->shaders_error) {
		reportFatalError("Error loading OpenGL shaders");
	}

	// OpenGL env
	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	CHECK_OPENGL_ERROR
}


/**
* Sets the view mode (above, behind, etc)
* Informs the player entities of first-person vs third-person
**/
void RenderOpenGL::setViewMode(int viewmode)
{
	this->viewmode = viewmode;
	
	if (viewmode == GameSettings::firstPerson) {
		for (unsigned int i = 0; i < this->st->num_local; i++) {
			if (this->st->local_players[i]->p != NULL) {
				this->st->local_players[i]->p->viewModeFirstPerson();
			}
		}
	} else {
		for (unsigned int i = 0; i < this->st->num_local; i++) {
			if (this->st->local_players[i]->p != NULL) {
				this->st->local_players[i]->p->viewModeThirdPerson();
			}
		}
	}
}


/**
* Set the mouse grab status
**/
void RenderOpenGL::setMouseGrab(bool newval)
{
	#ifndef SDL1_VIDEO
		SDL_SetRelativeMouseMode(newval ? SDL_TRUE : SDL_FALSE);
		SDL_SetWindowGrab(this->window, newval ? SDL_TRUE : SDL_FALSE);
	#endif
}


/**
* Load game fonts (and also the base shaders needed by the menu)
*
* Strictly this isn't required, as fonts are loaded on-demand,
* But it should help avoid random dips in framerate the first
* time a font is used.
**/
void RenderOpenGL::loadFonts()
{
	this->getFont("DejaVuSans", 20.0f);
	
	// ModManager loads the fonts after mod loading but before
	// the menu has loaded menu needs some shaders loaded too.
	// This is a hack of a place to put this, but it works.
	this->loadShaders();
	if (this->shaders_error) {
		reportFatalError("Error loading OpenGL shaders");
	}
}


/**
* Load a font, or fetch an already loaded font
**/
OpenGLFont* RenderOpenGL::getFont(string name, float size)
{
	std::ostringstream key;
	key << name;
	key << size;

	std::map<std::string, OpenGLFont*>::iterator it = this->fonts.find(key.str());
	if (it != this->fonts.end()) {
		return it->second;
	} else {
		OpenGLFont* fnt = new OpenGLFont(this, name, GEng()->mm->getBase(), size);
		this->fonts.insert(std::pair<std::string, OpenGLFont*>(key.str(), fnt));
		return fnt;
	}
}


/**
* Init the rendering part of guichan
**/
void RenderOpenGL::initGuichan(gcn::Gui * gui, Mod * mod)
{
	gcn::OpenGLGraphics* graphics;
	gcn::ChaoticRageOpenGLSDLImageLoader* imageLoader;

	delete(gcn::Image::getImageLoader());
	imageLoader = new gcn::ChaoticRageOpenGLSDLImageLoader(mod);
	gcn::Image::setImageLoader(imageLoader);

	delete(gui_font);
	gui_font = new OpenGLFont(this, "DejaVuSans", mod, 12.0f * GEng()->gui_scale);
	
	try {
		gcn::Widget::setGlobalFont(gui_font);
	} catch (const gcn::Exception & ex) {
		reportFatalError(ex.getMessage());
	}

	delete(gui->getGraphics());
	graphics = new gcn::OpenGLGraphics(this->real_width, this->real_height);
	gui->setGraphics(graphics);
}


/**
* Set up the OpenGL viewport
* Doesn't run for single-screen (but you can trick it with of = 0)
*
* s = Screen no
* of = Total number of screens
**/
void RenderOpenGL::mainViewport(int s, int of)
{
	float w = (float)this->real_width;
	float h = (float)this->real_height;
	float x = 0.0f;
	float y = 0.0f;

	if (of == 2) {
		h /= 2.0f;
		y = (s == 1 ? 0.0f : h);
	}

	glViewport(x, y, w, h);

	float fov, dnear, dfar;
	if (eff_viewmode == GameSettings::firstPerson) {
		fov = 90.0f;
		dnear = 0.1f;
		dfar = 350.0f;
	} else if (eff_viewmode == GameSettings::behindPlayer) {
		fov = 60.0f;
		dnear = 1.0f;
		dfar = 350.0f;
	} else {
		fov = 45.0f;
		dnear = 1.0f;
		dfar = 350.0f;
	}

	// Projection for 3D stuff
	this->projection = glm::perspective(fov, w / h, dnear, dfar);

	// Ortho for gameplay HUD (fixed height, variable width)
	this->virt_height = 1000;
	this->virt_width = (int) floor(1000.0f * w / h);
	this->ortho = glm::ortho<float>(0.0f, (float)this->virt_width, (float)this->virt_height, 0.0f, -1.0f, 1.0f);

	// Ortho for guichan (real screen size)
	this->ortho_guichan = glm::ortho<float>(0.0f, (float)this->real_width, (float)this->real_height, 0.0f, -1.0f, 1.0f);
}


/**
* Calculate the raycast start and end vectors in world space for mouse picking
*
* It starts with coords in Normalized Device Coordinates, then multiplies
* by the inverse of the projectionview matrix to give the world coordinates
*
* TODO: test on split screen
**/
void RenderOpenGL::mouseRaycast(int x, int y, btVector3& start, btVector3& end)
{
	const float ray_length = 500.0f;
	
	glm::vec4 ray_nds(
		((2.0f * x) / this->real_width) - 1.0f,
		1.0f - ((2.0f * y) / this->real_height),
		1.0f,
		1.0f
	);

	glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);

	glm::vec4 ray_eye = glm::inverse(this->projection) * ray_clip;
	ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

	glm::vec3 ray_world = glm::vec3(glm::inverse(this->view) * ray_eye);
	ray_world = glm::normalize(ray_world);

	// Start is camera position
	start = btVector3(this->camera.x, this->camera.y, this->camera.z);

	// End is start + direction
	end = start + (btVector3(ray_world.x, ray_world.y, ray_world.z) * ray_length);
}


/**
* Enable physics debug drawing. OpenGL only, not supported on GLES.
**/
void RenderOpenGL::setPhysicsDebug(bool status)
{
#ifdef USE_DEBUG_DRAW
	if (status) {
		this->physicsdebug = new GLDebugDrawer();
		this->physicsdebug->setDebugMode(
			btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawAabb |
			btIDebugDraw::DBG_DrawContactPoints | btIDebugDraw::DBG_NoDeactivation |
			btIDebugDraw::DBG_DrawConstraints | btIDebugDraw::DBG_DrawConstraintLimits
		);
		this->st->physics->getWorld()->setDebugDrawer(this->physicsdebug);
	} else {
		this->st->physics->getWorld()->setDebugDrawer(NULL);
		delete this->physicsdebug;
		this->physicsdebug = NULL;
	}
#endif
}


/**
* Get current physics debug status
**/
bool RenderOpenGL::getPhysicsDebug()
{
	return (this->physicsdebug != NULL);
}


/**
* Saves a screenshot.
* Filename does NOT include extension
**/
void RenderOpenGL::saveScreenshot(string filename)
{
	SDL_Surface * surf = SDL_CreateRGBSurface(SDL_SWSURFACE, this->real_width, this->real_height, 24, 0x000000FF, 0x0000FF00, 0x00FF0000, 0);
	if (surf == NULL) return;

	glReadPixels(0, 0, this->real_width, this->real_height, GL_RGB, GL_UNSIGNED_BYTE, surf->pixels);

	SDL_Surface * flip = flipVert(surf);
	if (flip == NULL) return;
	SDL_FreeSurface(surf);

	SDL_SaveBMP(flip, filename.c_str());

	SDL_FreeSurface(flip);
}


/**
* Internal sprite loading from a SDL_RWops
**/
SpritePtr RenderOpenGL::loadSpriteFromRWops(SDL_RWops *rw, string filename)
{
	SDL_Surface * surf;

	surf = IMG_Load_RW(rw, 0);
	if (surf == NULL) {
		GL_LOG("Couldn't load texture '%s'", filename.c_str());
		load_err = true;
		return NULL;
	}

	// Checks
	if ((surf->w & (surf->w - 1)) != 0) {
		GL_LOG("Texture '%s' width is not a power of 2", filename.c_str());
		load_err = true;
		return NULL;
	}

	if ((surf->h & (surf->h - 1)) != 0) {
		GL_LOG("Texture '%s' height is not a power of 2", filename.c_str());
		load_err = true;
		return NULL;
	}

	if (surf->format->BytesPerPixel != 4 && surf->format->BytesPerPixel != 3) {
		GL_LOG("Texture '%s' not in 32-bit or 24-bit colour; unable to load into OpenGL", filename.c_str());
		load_err = true;
		return NULL;
	}

	// Create the sprite object
	SpritePtr sprite = new Sprite();
	sprite->w = surf->w;
	sprite->h = surf->h;
	sprite->orig = surf;

	this->surfaceToOpenGL(sprite);

	// Windows Only: Keep original in RAM for re-loading if the context is destroyed
	#ifndef _WIN32
		SDL_FreeSurface(sprite->orig);
		sprite->orig = NULL;
	#endif

	loaded.push_back(sprite);

	return sprite;
}


/**
* Loads an SDL_Surface into an OpenGL texture
**/
void RenderOpenGL::surfaceToOpenGL(SpritePtr sprite)
{
	GLenum texture_format;
	GLenum target_format;
	GLint num_colors;

	CHECK_OPENGL_ERROR

	// Determine OpenGL import type
	num_colors = sprite->orig->format->BytesPerPixel;
	if (num_colors == 4) {
		if (sprite->orig->format->Rmask == 0x000000ff) {
			texture_format = GL_RGBA;
			target_format = GL_RGBA;
		} else {
			texture_format = target_format = 0;
			assert(0);
		}

	} else if (num_colors == 3) {
		if (sprite->orig->format->Rmask == 0x000000ff) {
			texture_format = GL_RGB;
			target_format = GL_RGB;
		} else {
			texture_format = target_format = 0;
			assert(0);
		}

	} else {
		texture_format = target_format = 0;
		assert(0);
	}

	// Load texture
	sprite->pixels = this->loadTextureRBGA(sprite->orig->pixels, texture_format, GL_UNSIGNED_BYTE, sprite->w, sprite->h);

	CHECK_OPENGL_ERROR
}


/**
* Load a texture from an array of pixel data
**/
Uint32 RenderOpenGL::loadTextureRBGA(void* data, Uint32 format, Uint32 type, int w, int h)
{
	GLuint handle;

	// Create and bind texture handle
	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle);

	// Set stretching properties
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->mag_filter);

	if (this->settings->tex_aniso >= 1.0f) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, this->settings->tex_aniso);
	}

	// Load and create mipmaps
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, format, type, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	return handle;
}


/**
* Load a 1D texture
**/
SpritePtr RenderOpenGL::load1D(string filename, Mod* mod)
{
	SpritePtr tex;
	SDL_Surface* surf;
	GLenum texture_format;
	GLenum target_format;

	CHECK_OPENGL_ERROR;
	DEBUG("vid", "Loading 1D texture '%s'", filename.c_str());

	tex = new Sprite();

	glGenTextures(1, &tex->pixels);
	glBindTexture(GL_TEXTURE_1D, tex->pixels);

	// Open rwops
	SDL_RWops* rw = mod->loadRWops(filename);
	if (rw == NULL) {
		glDeleteTextures(1, &tex->pixels);
		delete(tex);
		return NULL;
	}

	// Read file
	surf = IMG_Load_RW(rw, 0);
	if (surf == NULL) {
		glDeleteTextures(1, &tex->pixels);
		delete(tex);
		SDL_RWclose(rw);
		return NULL;
	}

	// Check height is exactly 1 pixel
	if (surf->h != 1) {
		glDeleteTextures(1, &tex->pixels);
		delete(tex);
		SDL_RWclose(rw);
		return NULL;
	}

	target_format = GL_RGBA;

	if (surf->format->BytesPerPixel == 4) {
		if (surf->format->Rmask == 0x000000ff) {
			texture_format = GL_RGBA;
		} else {
			texture_format = 0;
			assert(0);
		}

	} else if (surf->format->BytesPerPixel == 3) {
		if (surf->format->Rmask == 0x000000ff) {
			texture_format = GL_RGB;
		} else {
			texture_format = 0;
			assert(0);
		}

	} else {
		glDeleteTextures(1, &tex->pixels);
		delete(tex);
		SDL_FreeSurface(surf);
		SDL_RWclose(rw);
		return NULL;
	}

	// Load image into] cubemap
	glTexImage1D(GL_TEXTURE_1D, 0, target_format, surf->w, 0, texture_format, GL_UNSIGNED_BYTE, surf->pixels);

	// Free img
	SDL_FreeSurface(surf);
	SDL_RWclose(rw);

	glGenerateMipmap(GL_TEXTURE_1D);
	glBindTexture(GL_TEXTURE_1D, 0);

	CHECK_OPENGL_ERROR;
	return tex;
}


/**
* Load a cubemap
**/
SpritePtr RenderOpenGL::loadCubemap(string filename_base, string filename_ext, Mod * mod)
{
	SpritePtr cubemap;
	SDL_Surface* surf;
	GLenum texture_format;
	GLenum target_format;

	CHECK_OPENGL_ERROR;
	DEBUG("vid", "Loading cubemap '%s'", filename_base.c_str());

	cubemap = new Sprite();

	glGenTextures(1, &cubemap->pixels);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->pixels);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, this->min_filter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, this->mag_filter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	if (this->settings->tex_aniso >= 1.0f) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, this->settings->tex_aniso);
	}

	const char* faces[6] = { "posx", "negx", "posy", "negy", "posz", "negz" };

	int currWidth = 0;
	int currHeight = 0;

	for (unsigned int i = 0; i < 6; i++) {
		string filename = filename_base + faces[i] + filename_ext;

		// Open rwops
		SDL_RWops* rw = mod->loadRWops(filename);
		if (rw == NULL) {
			glDeleteTextures(1, &cubemap->pixels);
			delete(cubemap);
			return NULL;
		}

		// Read file
		surf = IMG_Load_RW(rw, 0);
		if (surf == NULL) {
			glDeleteTextures(1, &cubemap->pixels);
			delete(cubemap);
			SDL_RWclose(rw);
			return NULL;
		}

		// All images need to be the same size
		if (currWidth == 0) {
			currWidth = surf->w;
			currHeight = surf->h;
		} else {
			if (currWidth != surf->w || currHeight != surf->h) {
				glDeleteTextures(1, &cubemap->pixels);
				delete(cubemap);
				SDL_RWclose(rw);
				return NULL;
			}
		}

		target_format = GL_RGBA;

		if (surf->format->BytesPerPixel == 4) {
			if (surf->format->Rmask == 0x000000ff) {
				texture_format = GL_RGBA;
			} else {
				texture_format = 0;
				assert(0);
			}

		} else if (surf->format->BytesPerPixel == 3) {
			if (surf->format->Rmask == 0x000000ff) {
				texture_format = GL_RGB;
			} else {
				texture_format = 0;
				assert(0);
			}

		} else {
			glDeleteTextures(1, &cubemap->pixels);
			delete(cubemap);
			SDL_FreeSurface(surf);
			SDL_RWclose(rw);
			return NULL;
		}

		// Load image into] cubemap
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, target_format, surf->w, surf->h, 0, texture_format, GL_UNSIGNED_BYTE, surf->pixels);

		// Free img
		SDL_FreeSurface(surf);
		SDL_RWclose(rw);
	}

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	CHECK_OPENGL_ERROR;
	return cubemap;
}


/**
* Free sprite memory
**/
void RenderOpenGL::freeSprite(SpritePtr sprite)
{
	glDeleteTextures(1, &sprite->pixels);
	SDL_FreeSurface(sprite->orig);
	delete(sprite);
}


/**
* Create an OpenGL mesh for each heightmap in the map
**/
void RenderOpenGL::loadHeightmap()
{
	for (vector<Heightmap*>::iterator it = this->st->map->heightmaps.begin(); it != this->st->map->heightmaps.end(); ++it) {
		if ((*it)->isValid()) {
			(*it)->renderer = new RendererHeightmap(this, *it);
		}
	}
}


/**
* Free a loaded heightmap.
**/
void RenderOpenGL::freeHeightmap()
{
	for (vector<Heightmap*>::iterator it = this->st->map->heightmaps.begin(); it != this->st->map->heightmaps.end(); ++it) {
		delete (*it)->renderer;
	}
}


/**
* Create the "water" mesh
* We save it in a WavefrontObj.
**/
void RenderOpenGL::createWater()
{
	Vertex v;
	TexUV t;
	Face f;

	waterobj = new WavefrontObj();
	v.y = 0.0;

	v.x = 0.0f; v.z = 0.0f; t.x = 0.0f; t.y = 0.0f;
	waterobj->vertexes.push_back(v);
	waterobj->texuvs.push_back(t);

	v.x = 0.0f; v.z = 1.0f; t.x = 0.0f; t.y = 1.0f;
	waterobj->vertexes.push_back(v);
	waterobj->texuvs.push_back(t);

	v.x = 1.0f; v.z = 1.0f; t.x = 1.0f; t.y = 1.0f;
	waterobj->vertexes.push_back(v);
	waterobj->texuvs.push_back(t);

	v.x = 1.0f; v.z = 0.0f; t.x = 1.0f; t.y = 0.0f;
	waterobj->vertexes.push_back(v);
	waterobj->texuvs.push_back(t);

	v.x = 0.0f; v.y = 1.0f; v.z = 0.0f;
	waterobj->normals.push_back(v);

	f.v1 = 1; f.v2 = 2; f.v3 = 3;
	f.t1 = 1; f.t2 = 2; f.t3 = 3;
	f.n1 = 1; f.n2 = 1; f.n3 = 1;
	waterobj->faces.push_back(f);

	f.v1 = 1; f.v2 = 3; f.v3 = 4;
	f.t1 = 1; f.t2 = 3; f.t3 = 4;
	f.n1 = 1; f.n2 = 1; f.n3 = 1;
	waterobj->faces.push_back(f);
}


/**
* Create the "skybox" mesh
* We save it in a GLVAO.
**/
void RenderOpenGL::createSkybox()
{
	GLuint vertex;
	GLuint index;

	float vertexData[] = {
		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
	};

	Uint8 indexData[] = {0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1};

	// Create VAO
	skybox_vao = new GLVAO();

	// Index
	glGenBuffers(1, &index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);
	skybox_vao->setIndex(index);

	// Position
	glGenBuffers(1, &vertex);
	glBindBuffer(GL_ARRAY_BUFFER, vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	skybox_vao->setPosition(vertex);
}


/**
* Set up the shadow depth texture and the shadow framebuffer
**/
void RenderOpenGL::createShadowBuffers()
{
	// Create a texture and framebuffer
	glGenTextures(1, &this->shadow_depth_tex);
	glGenFramebuffers(1, &this->shadow_framebuffer);

	// Set up the texture
	glBindTexture(GL_TEXTURE_2D, this->shadow_depth_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, RenderOpenGL::SHADOW_MAP_WIDTH, RenderOpenGL::SHADOW_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (this->settings->tex_aniso >= 1.0f) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, this->settings->tex_aniso);
	}

	// Set up the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, this->shadow_framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->shadow_depth_tex, 0);

	// We don't draw to the framebuffer
	#ifdef OpenGL
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	#endif

	// Check it
	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		switch (status) {
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: GL_LOG("Framebuffer status %i GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT", status); break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: GL_LOG("Framebuffer status %i GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT", status); break;
			case GL_FRAMEBUFFER_UNSUPPORTED: GL_LOG("Framebuffer status %i GL_FRAMEBUFFER_UNSUPPORTED", status); break;
			default: GL_LOG("Framebuffer status %i unknown", status); break;
		}
		reportFatalError("Error creating shadow framebuffer");
	}

	// Done; return to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// But we *do* draw to the default one!
	#ifdef OpenGL
	glDrawBuffer(GL_BACK);
	glReadBuffer(GL_BACK);
	#endif
}


/**
* Renders a sprite.
* Should only be used if the the caller was called by this classes 'Render' function.
**/
void RenderOpenGL::renderSprite(SpritePtr sprite, int x, int y)
{
	renderSprite(sprite, x, y, sprite->w, sprite->h);
}

/**
* Renders a sprite.
* Should only be used if the the caller was called by this classes 'Render' function.
**/
void RenderOpenGL::renderSprite(SpritePtr sprite, int x, int y, int w, int h)
{
	this->renderSprite(sprite->pixels, x, y, w, h);
}


/**
* Renders a sprite.
* Should only be used if the the caller was called by this classes 'Render' function.
**/
void RenderOpenGL::renderSprite(GLuint texture, int x, int y, int w, int h)
{
	CHECK_OPENGL_ERROR;

	glBindTexture(GL_TEXTURE_2D, texture);

	// Draw a textured quad -- the image
	GLfloat box[4][5] = {
		{static_cast<float>(x), static_cast<float>(y) + h, 0.0f,      0.0f, 1.0f},
		{static_cast<float>(x + w), static_cast<float>(y) + h, 0.0f,  1.0f, 1.0f},
		{static_cast<float>(x), static_cast<float>(y), 0.0f,          0.0f, 0.0f},
		{static_cast<float>(x + w), static_cast<float>(y), 0.0f,      1.0f, 0.0f},
	};

	if (sprite_vbo == 0) {
		glGenVertexArrays(1, &sprite_vao);
		glGenBuffers(1, &sprite_vbo);
	}

	glBindVertexArray(sprite_vao);
	glBindBuffer(GL_ARRAY_BUFFER, sprite_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 20, BUFFER_OFFSET(0));
	glVertexAttribPointer(ATTRIB_TEXUV, 2, GL_FLOAT, GL_FALSE, 20, BUFFER_OFFSET(12));
	glEnableVertexAttribArray(ATTRIB_POSITION);
	glEnableVertexAttribArray(ATTRIB_TEXUV);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	CHECK_OPENGL_ERROR;
}


/**
* Load data which is common across multiple games
**/
void RenderOpenGL::loadCommonData()
{
	this->ambient_daynight = this->load1D("textures/ambient_daynight.png", GEng()->mm->getBase());
	if (this->ambient_daynight == NULL) {
		reportFatalError("Unable to read 'ambient_daynight.png' texture");
	}

	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_1D, this->ambient_daynight->pixels);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glActiveTexture(GL_TEXTURE0);
}


/**
* Various bits of set up before a game begins
* Some state variables get clobbered by the menu, so we have to set them here instead.
**/
void RenderOpenGL::preGame()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_CLAMP);
	glEnable(GL_CULL_FACE);

	if (this->settings->msaa >= 2) {
		glEnable(GL_MULTISAMPLE);
	} else {
		glDisable(GL_MULTISAMPLE);
	}

	CHECK_OPENGL_ERROR;

	// This will load the shaders (from the base mod) if they aren't loaded.
	this->loadShaders();
	if (this->shaders_error) {
		reportFatalError("Error loading OpenGL shaders");
	}
	
	// Set a few other vars
	this->render_player = NULL;
	this->torch = false;

	// Setup all the uniforms and such
	this->setupShaders();

	// Create some VBOs
	this->createWater();
	this->createSkybox();
	this->createShadowBuffers();
	
	// Set default camera at middle of map
	for (unsigned int i = 0; i < this->st->num_local; ++i) {
		this->last_origin[i] = btVector3(st->map->width/2.0f, 0.0f, st->map->height/2.0f);
	}
}


/**
* Set up shaders uniforms which are const throughout the game - lights, etc
*
* This used to be called once per game, but it's now once per player per frame
* to allow for dynamic lights
**/
void RenderOpenGL::setupShaders()
{
	glm::vec3 LightPos[4];
	glm::vec4 LightColor[4];
	unsigned int idx = 0;
	
	// TODO: Torch should be a directional light
	if (this->torch && this->render_player != NULL) {
		float inFront = 2.5f;
		btVector3 offGround = btVector3(0.0f, 0.3f, 0.0f);

		btTransform xform = this->render_player->getTransform();
		btVector3 pos = xform.getOrigin() + offGround + xform.getBasis() * btVector3(0.0f, 0.0f, inFront);

		LightPos[idx] = glm::vec3(pos.x(), pos.y() + 2.5f, pos.z());
		LightColor[idx] = glm::vec4(0.5f, 0.5f, 0.5f, 0.5f);
		idx++;
	}

	// TODO: Use the 4 closest light sources instead of just the first 4 in the list
	for (unsigned int i = 0; i < this->lights.size(); i++) {
		Light * l = this->lights[i];

		if (l->type == 3) {
			LightPos[idx] = glm::vec3(l->x, l->y, l->z);
			LightColor[idx] = glm::vec4(l->diffuse[0], l->diffuse[1], l->diffuse[2], 1.0f);
			idx++;
			if (idx == 4) break;
		}
	}

	// Make entities stand out a little when it's really dark
	glm::vec4 entityAmbient = this->ambient;
	if (st->time_of_day < 0.2f) {
		entityAmbient += 0.05;
	}

	// Update the uniforms for all shaders which have the 'lighting' flag set
	for (map<int, GLShader*>::iterator it = this->shaders.begin(); it != this->shaders.end(); ++it) {
		GLShader *shader = it->second;
		if (shader->getUniformsLighting()) {
			glUseProgram(shader->p());
			glUniform3fv(shader->uniform("uLightPos"), 4, glm::value_ptr(LightPos[0]));
			glUniform4fv(shader->uniform("uLightColor"), 4, glm::value_ptr(LightColor[0]));
			glUniform4fv(shader->uniform("uAmbient"), 1, glm::value_ptr(entityAmbient));
			glUniform1i(shader->uniform("uTex"), 0);
			glUniform1i(shader->uniform("uShadowMap"), 1);
			glUniform1i(shader->uniform("uNormal"), 2);
			glUniform1i(shader->uniform("uLightmap"), 3);
			glUniform1i(shader->uniform("uDayNight"), 4);
			glUniform1f(shader->uniform("uTimeOfDay"), st->time_of_day);
		}
	}

	for (vector<Heightmap*>::iterator it = this->st->map->heightmaps.begin(); it != this->st->map->heightmaps.end(); ++it) {
		// TODO: Set lighting in heightmap shaders too
	}

	// Water only gets the 'ambient' set
	glUseProgram(this->shaders[SHADER_WATER]->p());
	glUniform4fv(this->shaders[SHADER_WATER]->uniform("uAmbient"), 1, glm::value_ptr(this->ambient));

	// Skybox day/night texture
	glUseProgram(this->shaders[SHADER_SKYBOX]->p());
	glUniform1i(this->shaders[SHADER_SKYBOX]->uniform("uDayNight"), 4);
	glUniform1f(this->shaders[SHADER_SKYBOX]->uniform("uTimeOfDay"), st->time_of_day);
	
	CHECK_OPENGL_ERROR;
}


/**
* Clear memory we allocated
**/
void RenderOpenGL::postGame()
{
	delete(this->waterobj);

	// Delete any `AnimPlay`s we still own
	for (int i = this->animations.size() - 1; i >= 0; --i) {
		delete(this->animations.at(i).play);
	}
	this->animations.clear();

	// Delete any `Light`s we still own
	for (int i = this->lights.size() - 1; i >= 0; --i) {
		delete(this->lights.at(i));
	}
	this->lights.clear();

	particle_renderers.clear();
}


/**
* Clears all colour from a given pixel for a given sprite
**/
void RenderOpenGL::clearPixel(int x, int y)
{
}

/**
* Returns sprite width
**/
int RenderOpenGL::getSpriteWidth(SpritePtr sprite)
{
	return sprite->w;
}

/**
* Returns sprite height
**/
int RenderOpenGL::getSpriteHeight(SpritePtr sprite)
{
	return sprite->h;
}



/**
* Basic vertex shader for use before the base mod has been loaded
* Just textures, no lighting
**/
static const char* pVS = "\
attribute vec3 vPosition;   \n\
attribute vec2 vTexUV0;      \n\
varying vec2 fTexUV;        \n\
uniform mat4 uMVP;          \n\
void main() {               \n\
	gl_Position = uMVP * vec4(vPosition, 1.0); fTexUV = vTexUV0;   \n\
}";

/**
* Basic fragment shader for use before the base mod has been loaded
* Just textures, no lighting
**/
static const char* pFS = "\
varying vec2 fTexUV;       \n\
uniform sampler2D uTex;    \n\
void main() {              \n\
	gl_FragColor = texture2D(uTex, fTexUV);     \n\
}";


/**
* Load a pair of strings into a GLShader object
**/
GLShader* RenderOpenGL::createProgram(const char* vertex, const char* fragment)
{
	GLShader *s = new GLShader(0);

	bool result = s->createProgFromStrings(vertex, fragment);
	if (!result) {
		delete s;
		return NULL;
	}

	return s;
}


/**
* Load all of the general shaders we use for rendering
* Doesn't include various special-effects shaders that may be in use.
**/
void RenderOpenGL::loadShaders()
{
	Mod *base;

	// Don't even bother if they are already loaded
	if (this->shaders_loaded) return;

	this->shaders_error = false;

	// Before the mod is loaded, we only need the basic shader
	// It's are hardcoded (see above)
	if (! this->shaders.count(SHADER_BASIC)) {
		GLShader *shader = createProgram(pVS, pFS);
		if (shader == NULL) {
			reportFatalError("Error loading default OpenGL shader");
		}
		this->shaders[SHADER_BASIC] = shader;
	}

	// No mod loaded yet, can't load the shaders
	if (GEng()->mm == NULL) return;

	base = GEng()->mm->getBase();

	this->shaders[SHADER_ENTITY_BONES] = loadProgram(base, "phong_bones", "phong");
	this->shaders[SHADER_ENTITY_STATIC] = loadProgram(base, "phong_static", "phong");
	this->shaders[SHADER_ENTITY_STATIC_BUMP] = loadProgram(base, "phong_static", "phong_bump");
	this->shaders[SHADER_TERRAIN_PLAIN] = loadProgram(base, "phong_static", "phong_shadow");
	this->shaders[SHADER_TERRAIN_NORMALMAP] = loadProgram(base, "phong_normalmap", "phong_shadow");
	this->shaders[SHADER_WATER] = loadProgram(base, "water");
	this->shaders[SHADER_TEXT] = loadProgram(base, "text");
	this->shaders[SHADER_SKYBOX] = loadProgram(base, "skybox");

	this->shaders[SHADER_WATER]->setUniformsLighting(false);
	this->shaders[SHADER_TEXT]->setUniformsLighting(false);
	this->shaders[SHADER_SKYBOX]->setUniformsLighting(false);

	this->shaders_loaded = true;
}


/**
* Force a reload of the shaders
**/
bool RenderOpenGL::reloadShaders()
{
	map<int, GLShader*> old = map<int, GLShader*>(this->shaders);

	// Load new
	this->shaders.clear();
	this->shaders_loaded = false;
	this->loadShaders();
	
	// If error, revert to old
	if (this->shaders_error) {
		for (map<int, GLShader*>::iterator it = old.begin(); it != old.end(); ++it) {
			this->shaders[it->first] = it->second;
		}
		return false;
	}

	this->setupShaders();

	// Kill off the old shaders
	for (map<int, GLShader*>::iterator it = old.begin(); it != old.end(); ++it) {
		this->deleteProgram(it->second);
	}

	return true;
}


/**
* Load shaders using source found in a mod
*
* Will load the files:
*   shaders_[gl|es]/<name>.glslv (vertex)
*   shaders_[gl|es]/<name>.glslf (fragment)
**/
GLShader* RenderOpenGL::loadProgram(Mod* mod, string name)
{
	return this->loadProgram(mod, name, name);
}


/**
* Load shaders using source found in a mod
*
* Will load the files:
*   shaders_[gl|es]/<vertex_name>.glslv (vertex)
*   shaders_[gl|es]/<fragment_name>.glslf (fragment)
**/
GLShader* RenderOpenGL::loadProgram(Mod* mod, string vertex_name, string fragment_name)
{
	char* v;
	char* f;
	GLShader* s;

	v = mod->loadText("shaders_gl/" + vertex_name + ".glslv");
	f = mod->loadText("shaders_gl/" + fragment_name + ".glslf");
	
	if (v == NULL || f == NULL) {
		free(v);
		free(f);
		GL_LOG("Unable to load shader program %s %s", vertex_name.c_str(), fragment_name.c_str());
		this->shaders_error = true;
		return NULL;
	}

	CHECK_OPENGL_ERROR;

	s = this->createProgram(v, f);

	CHECK_OPENGL_ERROR;

	free(v);
	free(f);

	if (s == NULL) {
		GL_LOG("Unable to create shader program %s %s", vertex_name.c_str(), fragment_name.c_str());
		this->shaders_error = true;
	}

	return s;
}


/**
* Unlinks a shader, freeing OpenGL and system memory
* The pointer passed in the variable `shader` will become INVALID after this call is made.
**/
void RenderOpenGL::deleteProgram(GLShader* shader)
{
	glDeleteProgram(shader->p());
	delete(shader);
}



/**
* Builds a VBO for this object
**/
void RenderOpenGL::createVBO(WavefrontObj * obj)
{
	GLuint buffer;

	CHECK_OPENGL_ERROR;

	// Create VAO
	obj->vao = new GLVAO();
	obj->count = obj->faces.size() * 3;

	// Build the data array of interleaved data
	VBOvertex* vertexes = new VBOvertex[obj->faces.size() * 3];
	int j = 0;
	for (unsigned int i = 0; i < obj->faces.size(); i++) {
		Face * f = &obj->faces.at(i);

		Vertex * v;
		Vertex * vn;
		TexUV * t;

		v = &obj->vertexes.at(f->v1 - 1);
		vn = &obj->normals.at(f->n1 - 1);
		t = &obj->texuvs.at(f->t1 - 1);

		vertexes[j].x = v->x; vertexes[j].y = v->y; vertexes[j].z = v->z;
		vertexes[j].nx = vn->x; vertexes[j].ny = vn->y; vertexes[j].nz = vn->z;
		vertexes[j].tx = t->x; vertexes[j].ty = -t->y;
		j++;


		v = &obj->vertexes.at(f->v2 - 1);
		vn = &obj->normals.at(f->n2 - 1);
		t = &obj->texuvs.at(f->t2 - 1);

		vertexes[j].x = v->x; vertexes[j].y = v->y; vertexes[j].z = v->z;
		vertexes[j].nx = vn->x; vertexes[j].ny = vn->y; vertexes[j].nz = vn->z;
		vertexes[j].tx = t->x; vertexes[j].ty = -t->y;
		j++;


		v = &obj->vertexes.at(f->v3 - 1);
		vn = &obj->normals.at(f->n3 - 1);
		t = &obj->texuvs.at(f->t3 - 1);

		vertexes[j].x = v->x; vertexes[j].y = v->y; vertexes[j].z = v->z;
		vertexes[j].nx = vn->x; vertexes[j].ny = vn->y; vertexes[j].nz = vn->z;
		vertexes[j].tx = t->x; vertexes[j].ty = -t->y;
		j++;

	}

	// Create VBO
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VBOvertex) * obj->faces.size() * 3, vertexes, GL_STATIC_DRAW);
	obj->vao->setInterleavedPNT(buffer);

	CHECK_OPENGL_ERROR;

	delete [] vertexes;
}


/**
* Sorts so that AnimPlay* instances with the same model are next to each other in the array
**/
bool sorter(const PlayEntity& a, const PlayEntity& b)
{
	return ((unsigned long)(a.play->getModel()) < (unsigned long)(b.play->getModel()));
}


/**
* Adds an animation to the list
**/
void RenderOpenGL::addAnimPlay(AnimPlay* play, Entity* e)
{
	if (play->getModel() == NULL) return;

	PlayEntity ae;
	ae.play = play;
	ae.e = e;
	this->animations.push_back(ae);

	std::sort(this->animations.begin(), this->animations.end(), sorter);
}


/**
* Only temporary until we move the modelmatrix stuff into the AnimPlay class
**/
class DeleteIfPlay
{
	public:
		AnimPlay* del;
		DeleteIfPlay(AnimPlay* toDelete) {
			del = toDelete;
		}
		bool operator()( PlayEntity &ptr ) const {
			return (ptr.play == del);
		}
};


/**
* Remove an animation
**/
void RenderOpenGL::remAnimPlay(AnimPlay* play)
{
	this->animations.erase(
		remove_if(this->animations.begin(), this->animations.end(), DeleteIfPlay(play)),
		this->animations.end()
	);
}


/**
* Render the animations currently in the list
* We sort too because that makes OpenGL happier
**/
void RenderOpenGL::entities()
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->shadow_depth_tex);
	glActiveTexture(GL_TEXTURE0);

	// The uV (view) uniform only needs setting once per frame
	// Find shaders used in scene, and set the unform for them
	set<GLShader*> processed = set<GLShader*>();
	for (vector<PlayEntity>::iterator it = animations.begin(); it != animations.end(); ++it) {
		GLShader *shader = this->determineAssimpModelShader((*it).play->getModel());
		if (processed.find(shader) == processed.end()) {
			glUseProgram(shader->p());
			glUniformMatrix4fv(shader->uniform("uV"), 1, GL_FALSE, glm::value_ptr(this->view));
			processed.insert(shader);
		}
	}
	
	// Render!
	for (vector<PlayEntity>::iterator it = animations.begin(); it != animations.end(); ++it) {
		renderAnimPlay((*it).play, (*it).e);
	}

	CHECK_OPENGL_ERROR;
}


/**
* For a given model, use heuristics to determine the correct shader to use
*
* TODO: We might want a way for the model to *specify* the shader in the future
**/
GLShader* RenderOpenGL::determineAssimpModelShader(AssimpModel* am)
{
	if (!am->meshes[0]->bones.empty()) {
		return this->shaders[SHADER_ENTITY_BONES];
	}

	if (am->materials[am->meshes[0]->materialIndex]->normal != NULL) {
		return this->shaders[SHADER_ENTITY_STATIC_BUMP];
	}

	return this->shaders[SHADER_ENTITY_STATIC];
}


/**
* Renders an animation.
**/
void RenderOpenGL::renderAnimPlay(AnimPlay * play, Entity * e)
{
	float m[16];

	e->getTransform().getOpenGLMatrix(m);
	this->renderAnimPlay(play, glm::make_mat4(m));
}


/**
* Renders an animation.
**/
void RenderOpenGL::renderAnimPlay(AnimPlay* play, const glm::mat4 &modelMatrix)
{
	AssimpModel* am;
	GLShader* shader;

	am = play->getModel();

	shader = this->determineAssimpModelShader(am);
	glUseProgram(shader->p());

	// Combine the incoming position of the entity with the whole-model transform
	glm::mat4 localTransform = modelMatrix * am->transform;

	if (am->meshes[0]->bones.empty()) {
		// Static meshes are very easy
		recursiveRenderAssimpModelStatic(play, am, am->rootNode, shader, localTransform);

	} else {
		// Skinned meshes are a bit trickier
		glm::mat4 MVP = this->projection * this->view * localTransform;
		glm::mat4 depthBiasMVP = biasMatrix * this->depthmvp * localTransform;

		// Set uniforms
		glUniformMatrix4fv(shader->uniform("uMVP"), 1, GL_FALSE, glm::value_ptr(MVP));
		glUniformMatrix4fv(shader->uniform("uM"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix3fv(shader->uniform("uMN"), 1, GL_FALSE, glm::value_ptr(glm::inverseTranspose(glm::mat3(modelMatrix))));
		glUniformMatrix4fv(shader->uniform("uDepthBiasMVP"), 1, GL_FALSE, glm::value_ptr(depthBiasMVP));

		// Render the model
		recursiveRenderAssimpModelBones(play, am, am->rootNode, shader);
	}
}


/**
* Render an Assimp model.
*
* It's a recursive function because Assimp models have a node tree
* This is the 'static' version.
*
* @param AssimpModel *am The model
* @param AssimpNode *nd The root node of the model
* @param GLuint shader The bound shader, so uniforms will work
* @param glm::mat4 transform The node transform matrix
**/
void RenderOpenGL::recursiveRenderAssimpModelStatic(AnimPlay* ap, AssimpModel* am, AssimpNode* nd, GLShader* shader, const glm::mat4 &modelMatrix)
{
	glm::mat4 transform = modelMatrix * ap->getNodeTransform(nd);
	glm::mat4 MVP = this->projection * this->view * transform;
	glm::mat4 depthBiasMVP = biasMatrix * this->depthmvp * transform;

	// Set uniforms
	glUniformMatrix4fv(shader->uniform("uMVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	glUniformMatrix4fv(shader->uniform("uM"), 1, GL_FALSE, glm::value_ptr(transform));
	glUniformMatrix3fv(shader->uniform("uMN"), 1, GL_FALSE, glm::value_ptr(glm::inverseTranspose(glm::mat3(transform))));
	glUniformMatrix4fv(shader->uniform("uDepthBiasMVP"), 1, GL_FALSE, glm::value_ptr(depthBiasMVP));

	// Render meshes
	for (vector<unsigned int>::iterator it = nd->meshes.begin(); it != nd->meshes.end(); ++it) {
		AssimpMesh* mesh = am->meshes[(*it)];

		if (am->materials[mesh->materialIndex]->normal != NULL) {
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, am->materials[mesh->materialIndex]->normal->pixels);
			glActiveTexture(GL_TEXTURE0);
		}

		if (am->materials[mesh->materialIndex]->lightmap != NULL) {
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, am->materials[mesh->materialIndex]->lightmap->pixels);
			glActiveTexture(GL_TEXTURE0);
		}

		glActiveTexture(GL_TEXTURE0);
		if (am->materials[mesh->materialIndex]->diffuse == NULL) {
			glBindTexture(GL_TEXTURE_2D, 0);
		} else {
			glBindTexture(GL_TEXTURE_2D, am->materials[mesh->materialIndex]->diffuse->pixels);
		}

		mesh->vao->bind();
		glDrawElements(GL_TRIANGLES, mesh->numFaces*3, GL_UNSIGNED_SHORT, 0);
	}

	// Render child nodes
	for (vector<AssimpNode*>::iterator it = nd->children.begin(); it != nd->children.end(); ++it) {
		recursiveRenderAssimpModelStatic(ap, am, (*it), shader, modelMatrix);
	}
}


/**
* Render an Assimp model.
*
* It's a recursive function because Assimp models have a node tree
* This is the 'bones' version.
* The uMVP, uM, uV and uBones matrixes should be set *before* you call this
*
* @param AssimpModel *am The model
* @param AssimpNode *nd The root node of the model
* @param GLuint shader The bound shader, so uniforms will work
**/
void RenderOpenGL::recursiveRenderAssimpModelBones(AnimPlay* ap, AssimpModel* am, AssimpNode* nd, GLShader* shader)
{
	for (vector<unsigned int>::iterator it = nd->meshes.begin(); it != nd->meshes.end(); ++it) {
		AssimpMesh* mesh = am->meshes[(*it)];

		// Calculate and apply bone transforms
		ap->applyBoneTransforms(mesh);
		glUniformMatrix4fv(shader->uniform("uBones[0]"), MAX_BONES, GL_FALSE, &ap->bone_transforms[0][0][0]);

		if (am->materials[mesh->materialIndex]->diffuse == NULL) {
			glBindTexture(GL_TEXTURE_2D, 0);
		} else {
			glBindTexture(GL_TEXTURE_2D, am->materials[mesh->materialIndex]->diffuse->pixels);
		}

		mesh->vao->bind();
		glDrawElements(GL_TRIANGLES, mesh->numFaces*3, GL_UNSIGNED_SHORT, 0);
	}

	// Render child nodes
	for (vector<AssimpNode*>::iterator it = nd->children.begin(); it != nd->children.end(); ++it) {
		recursiveRenderAssimpModelBones(ap, am, (*it), shader);
	}
}


/**
* Add a light
**/
void RenderOpenGL::addLight(Light* light)
{
	this->lights.push_back(light);
}


/**
* Remove a light
**/
void RenderOpenGL::remLight(Light* light)
{
	this->lights.erase(std::remove(this->lights.begin(), this->lights.end(), light), this->lights.end());
}


/**
* Set the state of the torch
**/
void RenderOpenGL::setTorch(bool on)
{
	this->torch = on;
}


/**
* Set ambient light
**/
void RenderOpenGL::setAmbient(glm::vec4 ambient)
{
	this->ambient = ambient;
}


/**
* Draws text
*
* Note that the Y is for the baseline of the text.
**/
void RenderOpenGL::renderText(OpenGLFont* font, string text, int x, int y, float r, float g, float b, float a)
{
	font->drawString(NULL, text, x, y, r, g, b, a);
}


/**
* Returns the width of a string
**/
unsigned int RenderOpenGL::widthText(OpenGLFont* font, string text)
{
	return font->getWidth(text);
}


/**
* This is the main render function.
* It calls all of the sub-render parts which are defined below.
**/
void RenderOpenGL::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	entitiesCalcTransforms();
	entitiesShadowMap();

	for (unsigned int i = 0; i < this->st->num_local; i++) {
		this->render_player = this->st->local_players[i]->p;
		this->eff_viewmode = this->effectiveViewmode();

		this->mainViewport(i, this->st->num_local);
		this->mainRot(i);
		this->setupShaders();

		terrain();
		entities();
		skybox();
		particles();
		water();

		if (eff_viewmode == GameSettings::firstPerson) {
			weapon();
		}

		if (physicsdebug != NULL) physics();

		this->st->local_players[i]->hud->draw();
	}

	if (this->st->num_local != 1) {
		this->mainViewport(0, 1);
	}

	guichan();
	if (this->speeddebug) fps();

	CHECK_OPENGL_ERROR;

	#ifdef SDL1_VIDEO
		SDL_GL_SwapBuffers();
	#else
		SDL_GL_SwapWindow(this->window);
	#endif
}


/**
* Render debugging goodness for Bullet Physics
**/
void RenderOpenGL::physics()
{
#ifdef USE_DEBUG_DRAW
	CHECK_OPENGL_ERROR;

	glUseProgram(0);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);

	glLoadIdentity();
	glm::mat4 MVP = this->projection * this->view;
	glMultMatrixf(glm::value_ptr(MVP));

	st->physics->getWorld()->debugDrawWorld();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	CHECK_OPENGL_ERROR;
#endif
}


/**
* Determine the "effective" viewmode, i.e. tweaked in some curcimstances
**/
int RenderOpenGL::effectiveViewmode()
{
	if (this->render_player == NULL) {
		return GameSettings::behindPlayer;
	} else if (this->render_player->getWeaponZoom() > 0.0f) {
		return GameSettings::firstPerson;
	} else if (this->render_player->isInVehicle()) {
		return GameSettings::behindPlayer;
	} else {
		return this->viewmode;
	}
}


/**
* Main rotation for camera
**/
void RenderOpenGL::mainRot(unsigned int screen_index)
{
	CHECK_OPENGL_ERROR;

	btVector3 origin;
	float tilt, angle, dist;		// Up/down; left/right; distance of camera

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	
	if (this->render_player == NULL) {
		origin = this->last_origin[screen_index];
		tilt = 22.0f;
		dist = 80.0f;
		angle = st->game_time / 100.0f;

	} else {
		btTransform trans = this->render_player->getTransform();
		origin = trans.getOrigin();

		angle = this->render_player->getRenderAngle() + 180.0f;

		if (this->render_player->isDying()) {
			float perc = this->render_player->dyingPercent();
			tilt = 17.0f + (22.0f - 17.0f) * perc;
			dist = 25.0f + (80.0f - 25.0f) * perc;
			
		} else if (this->eff_viewmode == GameSettings::firstPerson) {
			tilt = 0.0f - this->render_player->vertical_angle;
			dist = 0.0f - this->render_player->getWeaponZoom();
			origin += btVector3(0.0f, UNIT_PHYSICS_HEIGHT / 2.3f, 0.0f);

		} else if (this->eff_viewmode == GameSettings::behindPlayer) {
			tilt = 17.0f;
			dist = 15.0f;

		} else if (this->eff_viewmode == GameSettings::abovePlayer) {
			tilt = 60.0f;
			dist = 50.0f;

		} else {
			// That's odd. Fix it.
			cerr << "ERROR: Wrong viewmode: " << this->viewmode << endl;
			this->viewmode = GameSettings::behindPlayer;
			tilt = 17.0f;
			dist = 25.0f;
		}

		this->last_origin[screen_index] = origin;
	}

	// Clamp angle
	while (angle < 0.0f) angle += 360.0f;
	while (angle > 360.0f) angle -= 360.0f;

	// Camera angle calculations
	float camerax = dist * sin(DEG_TO_RAD(angle)) * cos(DEG_TO_RAD(tilt)) + origin.x();
	float cameray = dist * sin(DEG_TO_RAD(tilt)) + origin.y();
	float cameraz = dist * cos(DEG_TO_RAD(angle)) * cos(DEG_TO_RAD(tilt)) + origin.z();
	this->camera = glm::vec3(camerax, cameray, cameraz);

	// Prep the view matrix
	this->view = glm::mat4(1.0f);
	this->view = glm::rotate(this->view, tilt, glm::vec3(1.0f, 0.0f, 0.0f));
	this->view = glm::rotate(this->view, 360.0f - angle, glm::vec3(0.0f, 1.0f, 0.0f));
	this->view = glm::translate(this->view, -this->camera);

	// Set the player position which is used in other areas
	this->render_player_pos = glm::vec3(origin.x(), origin.y(), origin.z());
	
	CHECK_OPENGL_ERROR;
}


/**
* Calculate the bone and animation transforms for all entity animations
**/
void RenderOpenGL::entitiesCalcTransforms()
{
	for (vector<PlayEntity>::iterator it = animations.begin(); it != animations.end(); ++it) {
		(*it).play->calcTransforms();
	}
}


/**
* Render the scene to the shadow map
*
* It's done from the perspective of the light source, and only depth is written
**/
void RenderOpenGL::entitiesShadowMap()
{
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, RenderOpenGL::SHADOW_MAP_WIDTH, RenderOpenGL::SHADOW_MAP_HEIGHT);
	glCullFace(GL_FRONT);

	// Bind and clear framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, shadow_framebuffer);
	#ifdef OpenGL
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	#endif
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Prep the view matrix
	btTransform trans;
	float tilt, angle, dist, lift;
	trans = btTransform(btQuaternion(0,0,0,0),btVector3(st->map->width/2.0f, 0.0f, st->map->height/2.0f));
	tilt = 22.0f;
	dist = 40.0f;
	lift = 0.0f;
	angle = 34.0f;

	// Camera angle calculations
	float camerax = dist * sin(DEG_TO_RAD(angle)) * cos(DEG_TO_RAD(tilt)) + trans.getOrigin().x();
	float cameray = dist * sin(DEG_TO_RAD(tilt)) + trans.getOrigin().y() + lift;
	float cameraz = dist * cos(DEG_TO_RAD(angle)) * cos(DEG_TO_RAD(tilt)) + trans.getOrigin().z();

	// Projection
	glm::mat4 depthProjectionMatrix = glm::ortho<float>(
		-150.0f, 150.0f,    // left <-> right
		-150.0f, 150.0f,    // bottom <-> top
		-10.0f, 150.0f      // near <-> far
	);

	// View
	glm::mat4 depthView = glm::mat4(1.0f);
	depthView = glm::rotate(depthView, tilt, glm::vec3(1.0f, 0.0f, 0.0f));
	depthView = glm::rotate(depthView, 360.0f - angle, glm::vec3(0.0f, 1.0f, 0.0f));
	depthView = glm::translate(depthView, glm::vec3(-camerax, -cameray, -cameraz));

	// Compute the MVP matrix from the light's point of view
	this->depthmvp = depthProjectionMatrix * depthView;
	this->projection = depthProjectionMatrix;
	this->view = depthView;

	// "Draw" entities to our FBO
	entities();

	// Reset everything for regular rendering
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	#ifdef OpenGL
	glDrawBuffer(GL_BACK);
	glReadBuffer(GL_BACK);
	#endif
	glCullFace(GL_BACK);
}


/**
* Render the skybox
**/
void RenderOpenGL::skybox()
{
	if (st->map->skybox == NULL) return;
	CHECK_OPENGL_ERROR;

	GLShader* s = this->shaders[SHADER_SKYBOX];

	glBindTexture(GL_TEXTURE_CUBE_MAP, st->map->skybox->pixels);
	glUseProgram(s->p());
	glCullFace(GL_FRONT);

	glm::mat4 modelMatrix = glm::mat4(1.0f);

	if (this->st->map->skybox_inf) {
		modelMatrix = glm::translate(modelMatrix, this->render_player_pos);
	} else {
		modelMatrix = glm::translate(modelMatrix, glm::vec3(st->map->skybox_size.x*0.5f, st->map->skybox_size.y*0.5f, st->map->skybox_size.z*0.5f));
	}

	modelMatrix = glm::scale(modelMatrix, st->map->skybox_size);
	modelMatrix = glm::rotate(modelMatrix, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 MVP = this->projection * this->view * modelMatrix;
	glUniformMatrix4fv(s->uniform("uMVP"), 1, GL_FALSE, glm::value_ptr(MVP));

	skybox_vao->bind();
	glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_BYTE, 0);

	glCullFace(GL_BACK);
	glUseProgram(0);
	CHECK_OPENGL_ERROR;
}


/**
* Terrain heightmap
**/
void RenderOpenGL::terrain()
{
	CHECK_OPENGL_ERROR;

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->shadow_depth_tex);
	glActiveTexture(GL_TEXTURE0);

	// Heightmaps
	for (vector<Heightmap*>::iterator it = this->st->map->heightmaps.begin(); it != this->st->map->heightmaps.end(); ++it) {
		(*it)->renderer->draw(this);
	}

	// Geomerty meshes
	GLShader* s = this->shaders[SHADER_TERRAIN_PLAIN];
	glUseProgram(s->p());
	for (vector<MapMesh*>::iterator it = st->map->meshes.begin(); it != st->map->meshes.end(); ++it) {
		MapMesh* mm = (*it);

		glm::mat4 MVP = this->projection * this->view * mm->xform;
		glm::mat4 depthBiasMVP = biasMatrix * this->depthmvp * mm->xform;

		glUniformMatrix4fv(s->uniform("uMVP"), 1, GL_FALSE, glm::value_ptr(MVP));
		glUniformMatrix4fv(s->uniform("uV"), 1, GL_FALSE, glm::value_ptr(this->view));
		glUniformMatrix4fv(s->uniform("uDepthBiasMVP"), 1, GL_FALSE, glm::value_ptr(depthBiasMVP));

		recursiveRenderAssimpModelStatic(mm->play, mm->model, mm->model->rootNode, s, mm->xform);
	}

	glUseProgram(0);

	CHECK_OPENGL_ERROR;
}


/**
* Water goes last because of we do fun things with blending
**/
void RenderOpenGL::water()
{
	if (this->st->map->water == NULL) return;

	CHECK_OPENGL_ERROR;

	glEnable(GL_BLEND);

	GLShader* s = this->shaders[SHADER_WATER];

	glBindTexture(GL_TEXTURE_2D, this->st->map->water->pixels);
	glUseProgram(s->p());

	if (this->waterobj->count == 0) this->createVBO(this->waterobj);

	glm::mat4 modelMatrix = glm::scale(
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, this->st->map->water_level, 0.0f)),
		glm::vec3(this->st->map->width, 1.0f, this->st->map->height)
	);

	glm::mat4 MVP = this->projection * this->view * modelMatrix;
	glUniformMatrix4fv(s->uniform("uMVP"), 1, GL_FALSE, glm::value_ptr(MVP));

	this->waterobj->vao->bind();
	glDrawArrays(GL_TRIANGLES, 0, this->waterobj->count);

	glUseProgram(0);

	glDisable(GL_BLEND);

	CHECK_OPENGL_ERROR;
}


/**
* First person view of weapon
**/
void RenderOpenGL::weapon()
{
	if (this->render_player == NULL) return;
	
	WeaponType* wt = this->render_player->getWeaponTypeCurr();
	if (wt == NULL) return;
	if (wt->model == NULL) return;
	
	glm::mat4 weaponModelMatrix = glm::translate(
		glm::inverse(this->view),
		glm::vec3(0.15f, -0.30f, -0.15f)   // left-right, down-up, forward-back
	);

	// TODO: Should the models just be rotated instead?
	weaponModelMatrix = glm::rotate(weaponModelMatrix, 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	AnimPlay* ap = new AnimPlay(wt->model);
	this->renderAnimPlay(ap, weaponModelMatrix);
	delete ap;
	
	// Render attachments too
	for (int i = 0; i < NUM_WPATT; ++i) {
		WeaponAttachmentLocation loc = static_cast<WeaponAttachmentLocation>(i);
		WeaponAttachment *wa = this->render_player->getAttachment(loc);
		if (wa == NULL) continue;
		
		glm::mat4 attModelMatrix = glm::translate(weaponModelMatrix, wt->attach_loc[i]);
		attModelMatrix = glm::translate(attModelMatrix, wa->loc[i]);
		
		AnimPlay* ap = new AnimPlay(wa->model);
		this->renderAnimPlay(ap, attModelMatrix);
		delete ap;
	}
}


void RenderOpenGL::addParticleRenderer(SPK::Renderer* r)
{
	particle_renderers.push_back(r);
}


/**
* Particle effects using SPARK
**/
void RenderOpenGL::particles()
{
	glEnable(GL_BLEND);

	// TODO: Should this be moved into SPK::System and only called for used renderers?
	glm::mat4 vp = this->projection * this->view;
	std::vector<SPK::Renderer*>::iterator it;
	for (it = particle_renderers.begin() ; it != particle_renderers.end(); ++it) {
		(*it)->setVP(this->view, vp);
	}

	this->st->particle_system->render();

	CHECK_OPENGL_ERROR;
}


/**
* Guichan dialogs
**/
void RenderOpenGL::guichan()
{
	if (GEng()->hasDialogs()) {
		GEng()->gui->draw();
	}
}


/**
* FPS counter - a testing option
**/
void RenderOpenGL::fps()
{
	char buf[BUFFER_MAX];
	float tick = GEng()->getAveTick();

	OpenGLFont* font = this->getFont("DejaVuSans", 20.0f);
	if (font == NULL) {
		return;
	}

	snprintf(buf, BUFFER_MAX, "%.2f ms", tick);
	this->renderText(font, buf, 400, 50);

	snprintf(buf, BUFFER_MAX, "%.1f fps", 1000.0f/tick);
	this->renderText(font, buf, 550, 50);

	// Count up entities
	int units = 0;
	int vehicles = 0;
	int objects = 0;
	int pickups = 0;
	int other = 0;
	int total = 0;
	for (list<Entity*>::iterator it = st->entities.begin(); it != st->entities.end(); ++it) {
		Entity *e = (*it);
		if (e->klass() == VEHICLE) {
			vehicles++;
		} else if (e->klass() == OBJECT) {
			objects++;
		} else if (e->klass() == PICKUP) {
			pickups++;
		} else if (e->klass() == UNIT) {
			units++;
		} else {
			other++;
		}
		total++;
	}

	snprintf(buf, BUFFER_MAX, "U %i  V %i  O %i  P %i  ? %i  T %i", units, vehicles, objects, pickups, other, total);
	this->renderText(font, buf, 400, 80);
}
