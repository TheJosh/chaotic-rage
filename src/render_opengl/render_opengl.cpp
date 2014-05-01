// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <algorithm>
#include <map>
#include "gl.h"
#include <SDL_image.h>
#include <math.h>

#include "../rage.h"
#include "../physics_bullet.h"
#include "../game_state.h"
#include "../game_engine.h"
#include "../entity/player.h"
#include "../entity/vehicle.h"
#include "../util/obj.h"
#include "../util/sdl_util.h"
#include "../util/windowicon.h"
#include "../mod/mod_manager.h"
#include "../mod/vehicletype.h"
#include "render_opengl.h"
#include "render_opengl_settings.h"
#include "gl_debug.h"
#include "gl_debug_drawer.h"
#include "glvao.h"
#include "../render/sprite.h"
#include "glshader.h"
#include "assimpmodel.h"
#include "animplay.h"
#include "hud.h"

#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <guichan/opengl.hpp>
#include "guichan_imageloader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef USE_SPARK
#include "../spark/SPK.h"
#include "../spark/SPK_GL.h"
#endif

#include <ft2build.h>
#include FT_FREETYPE_H

using namespace std;


/**
* For VBO pointer offsets
**/
#define BUFFER_OFFSET(i) ((char *)NULL + (i))


/**
* Gets the next highest power-of-two for a number
**/
static inline int next_pot (int a)
{
	int rval=1; 
	while(rval<a) rval<<=1;
	return rval;
}


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
	this->physicsdebug = NULL;
	this->speeddebug = false;
	this->viewmode = 0;
	this->face = NULL;

	#ifdef USE_SPARK
	this->particle_renderer = new SPK::GL::GL2PointRenderer(1.0f);
	this->st->particle_renderer = this->particle_renderer;
	#endif

	this->font_vbo = 0;
	this->sprite_vbo = 0;
	
	this->ter_vao = NULL;
	this->skybox_vao = NULL;
	
	// TODO: Do we need this? SDL2
	/*const SDL_VideoInfo* mode = SDL_GetVideoInfo();
	this->desktop_width = mode->current_w;
	this->desktop_height = mode->current_h;*/
	
	for (unsigned int i = 0; i < NUM_CHAR_TEX; i++) {
		this->char_tex[i].tex = 0;
	}
	
	this->settings = NULL;
	this->setSettings(settings);
	
	shaders_loaded = false;
}


/**
* Cleanup
**/
RenderOpenGL::~RenderOpenGL()
{
	delete(this->settings);
	
	#ifdef USE_SPARK
		delete(this->particle_renderer);
	#endif
	
	SDL_GL_DeleteContext(this->glcontext);

	// TODO: Delete all buffers, tex, etc.
}



/**
* Set the settings to use
**/
void RenderOpenGL::setSettings(RenderOpenGLSettings* settings)
{
	delete(this->settings);
	this->settings = settings;
	
	// Texture filtering modes
	switch (this->settings->tex_filter) {
		case 4:
			this->min_filter = GL_LINEAR_MIPMAP_LINEAR; this->mag_filter = GL_LINEAR;		// trilinear
			break;
			
		case 3:
			this->min_filter = GL_LINEAR_MIPMAP_NEAREST; this->mag_filter = GL_LINEAR;		// bilinear
			break;
			
		case 2:
			this->min_filter = GL_LINEAR_MIPMAP_NEAREST; this->mag_filter = GL_NEAREST;		// bilinear
			break;
			
		default:
			this->min_filter = GL_NEAREST_MIPMAP_NEAREST; this->mag_filter = GL_NEAREST;	// crappy
			break;
	}
	
	// Update tex with new filtering modes
	if (loaded.size() > 0) {
		for (unsigned int i = 0; i != loaded.size(); i++) {
			glBindTexture(GL_TEXTURE_2D, loaded[i]->pixels);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->min_filter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->mag_filter);
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
	int flags;
	
	// On mobile devices, we force fullscreen and a given window size
	#if defined(__ANDROID__)
		SDL_DisplayMode mode;
		
		int result = SDL_GetDesktopDisplayMode(0, &mode);
		if (result != 0) {
			reportFatalError("Unable to determine current display mode");
		}
		
		width = mode.w;
		height = mode.h;
		fullscreen = true;
	#endif
	
	this->real_width = width;
	this->real_height = height;
	
	// SDL flags
	flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
	if (fullscreen) {
		flags |= SDL_WINDOW_FULLSCREEN;
	}
	
	// Window title
	char title[100];
	sprintf(title, "Chaotic Rage %s", VERSION);
	
	// Create window
	this->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
	if (this->window == NULL) {
		char buffer[200];
		sprintf(buffer, "Unable to set %ix%i video: %s\n", width, height, SDL_GetError());
		reportFatalError(buffer);
	}
	
	// GL context settings
	#if defined(GLES)
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		
	#elif defined(OpenGL)
		if (this->settings->msaa >= 2) {
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, this->settings->msaa);
		} else {
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
		}
	#endif
	
	// GL context creation
	this->glcontext = SDL_GL_CreateContext(this->window);
	if (this->glcontext == NULL) {
		reportFatalError("Unable to create GL context");
	}
	
	// SDL_Image
	flags = IMG_INIT_PNG;
	int initted = IMG_Init(flags);
	if ((initted & flags) != flags) {
		reportFatalError("Failed to init required png support.");
	}
	
	// Check compat an init GLEW
	#ifdef OpenGL
		if (atof((char*) glGetString(GL_VERSION)) < 3.1) {
			reportFatalError("OpenGL 3.1 or later is required for the high-end renderer, but not supported on this system. Try the low-end one.");
		}
		
		GLenum err = glewInit();
		if (GLEW_OK != err) {
			GL_LOG("Glew Error: %s", glewGetErrorString(err));
			reportFatalError("Unable to init the library GLEW.");
		}
		
		// GL_ARB_framebuffer_object -> shadows and glGenerateMipmap
		if (! GL_ARB_framebuffer_object) {
			reportFatalError("OpenGL 3.0 or the extension 'GL_ARB_framebuffer_object' not available.");
		}
	#endif
	
	// Freetype
	int error;
	error = FT_Init_FreeType(&this->ft);
	if (error) {
		reportFatalError("Freetype: Unable to init library.");
	}
	
	#ifdef USE_SPARK
		this->particle_renderer->initGLbuffers();
	#endif
	
	// Windows only: Re-load textures.
	// All other platforms don't destory the context if the window size changes
	#ifdef _WIN32
		if (loaded.size() > 0) {
			for (unsigned int i = 0; i != loaded.size(); i++) {
				this->surfaceToOpenGL(loaded.at(i));
			}
		}
	#endif

	// Re-load FreeType character textures
	for (unsigned int i = 0; i < NUM_CHAR_TEX; i++) {
		if (this->char_tex[i].tex) {
			glDeleteTextures(1, &this->char_tex[i].tex);
			this->char_tex[i].tex = 0;
		}
	}
	
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
* Set the mouse grab status
**/
void RenderOpenGL::setMouseGrab(bool newval)
{
	SDL_SetRelativeMouseMode(newval ? SDL_TRUE : SDL_FALSE);
	SDL_SetWindowGrab(this->window, newval ? SDL_TRUE : SDL_FALSE);
}


/**
* Load a font using freetype
**/
void RenderOpenGL::loadFont(string name, Mod * mod)
{
	int error;
	Sint64 len;
	
	Uint8 *buf = mod->loadBinary(name, &len);
	if (buf == NULL) {
		reportFatalError("Freetype: Unable to load data");
	}
	
	// TODO: You've got to free buf at some point, but only after all the character bitmaps have been loaded
	// perhaps re-think this all a bit.
	
	error = FT_New_Memory_Face(this->ft, (const FT_Byte *) buf, len, 0, &this->face);

	if (error == FT_Err_Unknown_File_Format) {
		reportFatalError("Freetype: Unsupported font format");
	} else if (error) {
		reportFatalError("Freetype: Unable to load font");
	}
	
	error = FT_Set_Char_Size(this->face, 0, 20*64, 72, 72);
	if (error) {
		reportFatalError("Freetype: Unable to load font size");
	}

	// I don't quite know ahy this is here...
	// TODO: Move or remove
	this->loadShaders();
	if (this->shaders_error) {
		reportFatalError("Error loading OpenGL shaders");
	}
}


/**
* Init the rendering part of guichan
**/
void RenderOpenGL::initGuichan(gcn::Gui * gui, Mod * mod)
{
	gcn::OpenGLGraphics* graphics;
	gcn::ChaoticRageOpenGLSDLImageLoader* imageLoader;
	gcn::ImageFont* font;
	
	imageLoader = new gcn::ChaoticRageOpenGLSDLImageLoader(mod);
	gcn::Image::setImageLoader(imageLoader);
	
	try {
		// If you update the font, you gotta do in both cr and menu mods
		font = new gcn::ImageFont("fixedfont.bmp", " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789?!.,\"'_");
		gcn::Widget::setGlobalFont(font);
	} catch (gcn::Exception ex) {
		reportFatalError(ex.getMessage());
	}
	
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
	int w = this->real_width;
	int h = this->real_height;
	int x = 0;
	int y = 0;
	
	if (of == 2) {
		h /= 2;
		y = (s == 1 ? 0 : h);
	}
	
	this->virt_height = 1000;
	this->virt_width = (int) floor(this->virt_height * (float)w / (float)h);
	
	glViewport(x, y, w, h);
	
	this->projection = glm::perspective(45.0f, (float)this->virt_width / (float)this->virt_height, 1.0f, 350.0f);

	// Ortho for gameplay HUD etc
	this->ortho = glm::ortho<float>(0.0f, (float)this->virt_width, 0.0f, (float)this->virt_height, -1.0f, 1.0f);
}


/**
* Enable physics debug drawing. OpenGL only, not supported on GLES.
**/
void RenderOpenGL::setPhysicsDebug(bool status)
{
#ifdef OpenGL
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
SpritePtr RenderOpenGL::int_loadSprite(SDL_RWops *rw, string filename)
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
			assert(1); // TODO GLES removed: texture_format = GL_BGRA;
		}
		
	} else if (num_colors == 3) {
		if (sprite->orig->format->Rmask == 0x000000ff) {
			texture_format = GL_RGB;
			target_format = GL_RGB;
		} else {
			texture_format = target_format = 0;
			assert(1); // TODO GLES removed: texture_format = GL_BGR;
		}
		
	} else {
		texture_format = target_format = 0;
		assert(1);
	}

	#ifdef GLES
		target_format = texture_format;
	#endif
	
	// Create and bind texture handle
	glGenTextures(1, &sprite->pixels);
	glBindTexture(GL_TEXTURE_2D, sprite->pixels);
	
	// Set stretching properties
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->mag_filter);
	
	// Load and create mipmaps
	glTexImage2D(GL_TEXTURE_2D, 0, target_format, sprite->orig->w, sprite->orig->h, 0, texture_format, GL_UNSIGNED_BYTE, sprite->orig->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	CHECK_OPENGL_ERROR
}


/**
* Load a cubemap
**/
SpritePtr RenderOpenGL::loadCubemap(string filename_base, string filename_ext, Mod * mod)
{
	SpritePtr cubemap;
	SDL_RWops* rw;
	SDL_Surface* surf;
	GLenum texture_format;
	
	DEBUG("vid", "Loading cubemap '%s'", filename_base.c_str());
	
	cubemap = new Sprite();
	
	glGenTextures(1, &cubemap->pixels);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->pixels);
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, this->min_filter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, this->mag_filter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	// TODO: Look at this for GLES
	#ifdef OpenGL
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	#endif
	
	const char* faces[6] = { "posx", "negx", "posy", "negy", "posz", "negz" };
	
	for (unsigned int i = 0; i < 6; i++) {
		string filename = filename_base + faces[i] + filename_ext;
		
		rw = mod->loadRWops(filename);
		if (rw == NULL) {
			glDeleteTextures(1, &cubemap->pixels);
			return NULL;
		}
	
		surf = IMG_Load_RW(rw, 0);
		if (surf == NULL) {
			glDeleteTextures(1, &cubemap->pixels);
			SDL_RWclose(rw);
			return NULL;
		}
	
		SDL_RWclose(rw);
	
		if (surf->format->BytesPerPixel == 4) {
			if (surf->format->Rmask == 0x000000ff) {
				texture_format = GL_RGBA;
			} else {
				texture_format = 0;
				assert(1); // TODO GLES removed: texture_format = GL_BGRA;
			}
		
		} else if (surf->format->BytesPerPixel == 3) {
			if (surf->format->Rmask == 0x000000ff) {
				texture_format = GL_RGB;
			} else {
				texture_format = 0;
				assert(1); // TODO GLES removed: texture_format = GL_BGR;
			}
		
		} else {
			glDeleteTextures(1, &cubemap->pixels);
			SDL_FreeSurface(surf);
			return NULL;
		}
	
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, surf->w, surf->h, 0, texture_format, GL_UNSIGNED_BYTE, surf->pixels);
		SDL_FreeSurface(surf);
	}
	
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	
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
* Loads a heightmap from the raw heightmap data (an array of floats).
* TODO: This should use TRIANGLE_STRIPS not TRIANGLES for rendering.
**/
void RenderOpenGL::loadHeightmap()
{
	unsigned int nX, nZ, j;
	float flX, flZ;
	GLuint buffer;
	
	if (st->map->heightmap == NULL) st->map->createHeightmapRaw();
	if (st->map->heightmap == NULL) return;
	
	unsigned int maxX = st->map->heightmap_sx - 1;
	unsigned int maxZ = st->map->heightmap_sz - 1;

	this->ter_size = (maxX * maxZ * 2) + (maxZ * 2);
	VBOvertex* vertexes = new VBOvertex[this->ter_size];
	
	j = 0;
	for (nZ = 0; nZ < maxZ; nZ++) {
		for(nX = 0; nX < maxX; nX++) {
			
			// u = p2 - p1; v = p3 - p1
			btVector3 u = btVector3(nX + 1.0f, st->map->heightmapGet(nX + 1, nZ + 1), nZ + 1.0f) - btVector3(nX, st->map->heightmapGet(nX, nZ), nZ);
			btVector3 v = btVector3(nX + 1.0f, st->map->heightmapGet(nX + 1, nZ), nZ) - btVector3(nX, st->map->heightmapGet(nX, nZ), nZ);
			
			// calc vector
			btVector3 normal = btVector3(
				u.y() * v.z() - u.z() * v.y(),
				u.z() * v.x() - u.x() * v.z(),
				u.x() * v.y() - u.y() * v.x()
			);
			
			
			// First cell on the row has two extra verticies
			if (nX == 0) {
				flX = nX; flZ = nZ;
				vertexes[j].x = flX;
				vertexes[j].y = st->map->heightmapGet(flX, flZ);
				vertexes[j].z = flZ;
				vertexes[j].nx = normal.x();
				vertexes[j].ny = normal.y();
				vertexes[j].nz = normal.z();
				vertexes[j].tx = flX / st->map->heightmap_sx;
				vertexes[j].ty = flZ / st->map->heightmap_sz;
				j++;
				
				flX = nX; flZ = nZ + 1;
				vertexes[j].x = flX;
				vertexes[j].y = st->map->heightmapGet(flX, flZ);
				vertexes[j].z = flZ;
				vertexes[j].nx = normal.x();
				vertexes[j].ny = normal.y();
				vertexes[j].nz = normal.z();
				vertexes[j].tx = flX / st->map->heightmap_sx;
				vertexes[j].ty = flZ / st->map->heightmap_sz;
				j++;
			}
			
			// Top
			flX = nX + 1; flZ = nZ;
			vertexes[j].x = flX;
			vertexes[j].y = st->map->heightmapGet(flX, flZ);
			vertexes[j].z = flZ;
			vertexes[j].nx = normal.x();
			vertexes[j].ny = normal.y();
			vertexes[j].nz = normal.z();
			vertexes[j].tx = flX / st->map->heightmap_sx;
			vertexes[j].ty = flZ / st->map->heightmap_sz;
			j++;
			
			// Bottom
			flX = nX + 1; flZ = nZ + 1;
			vertexes[j].x = flX;
			vertexes[j].y = st->map->heightmapGet(flX, flZ);
			vertexes[j].z = flZ;
			vertexes[j].nx = normal.x();
			vertexes[j].ny = normal.y();
			vertexes[j].nz = normal.z();
			vertexes[j].tx = flX / st->map->heightmap_sx;
			vertexes[j].ty = flZ / st->map->heightmap_sz;
			j++;
		}
	}
	
	assert(j == this->ter_size);

	// Create VAO
	ter_vao = new GLVAO();
	
	// Create interleaved VBO
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VBOvertex) * this->ter_size, vertexes, GL_STATIC_DRAW);
	ter_vao->setInterleavedPNT(buffer);
	
	delete [] vertexes;
}


/**
* Free a loaded heightmap.
**/
void RenderOpenGL::freeHeightmap()
{
	delete ter_vao;
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
* Create the "water" mesh
* We save it in a WavefrontObj.
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
	
	unsigned int indexData[] = {0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1};
	
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
	#ifdef OpenGL
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	#endif
	glBindTexture(GL_TEXTURE_2D, 0);

	// Set up the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, this->shadow_framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->shadow_depth_tex, 0);
	
	// We don't draw to the framebuffer
	#ifdef OpenGL
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	#endif
	
	// Check it
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=  GL_FRAMEBUFFER_COMPLETE) {
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
		glGenBuffers(1, &sprite_vbo);
	}
	
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
* Various bits of set up before a game begins
* Some state variables get clobbered by the menu, so we have to set them here instead.
**/
void RenderOpenGL::preGame()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	#ifdef OpenGL
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		
		if (this->settings->msaa >= 2) {
			glEnable(GL_MULTISAMPLE);
		} else {
			glDisable(GL_MULTISAMPLE);
		}
	#endif
	
	CHECK_OPENGL_ERROR;
	
	// This will load the shaders (from the base mod) if they aren't loaded.
	this->loadShaders();
	if (this->shaders_error) {
		reportFatalError("Error loading OpenGL shaders");
	}
	
	// Setup all the uniforms and such
	this->setupShaders();

	// Create some VBOs
	this->createWater();
	this->createSkybox();
	this->createShadowBuffers();

	// Init the viewport for single screen only once
	if (this->st->num_local == 1) {
		this->mainViewport(1, 1);
	}
}


/**
* Set up shaders uniforms which are const throughout the game - lights, etc
**/
void RenderOpenGL::setupShaders()
{
	// Prep point lights...
	// TODO: Think about dynamic lights?
	glm::vec3 LightPos[2];
	glm::vec4 LightColor[2];
	unsigned int idx = 0;
	for (unsigned int i = 0; i < st->map->lights.size(); i++) {
		Light * l = st->map->lights[i];

		if (l->type == 3) {
			LightPos[idx] = glm::vec3(l->x, l->y, l->z);
			LightColor[idx] = glm::vec4(l->diffuse[0], l->diffuse[1], l->diffuse[2], 0.8f);
			idx++;
			if (idx == 2) break;
		}
	}
	
	// ...and ambient too
	glm::vec4 AmbientColor(this->st->map->ambient[0], this->st->map->ambient[1], this->st->map->ambient[2], 1.0f);

	// Assign to phong shader
	glUseProgram(this->shaders["phong"]->p());
	glUniform3fv(this->shaders["phong"]->uniform("uLightPos"), 2, glm::value_ptr(LightPos[0]));
	glUniform4fv(this->shaders["phong"]->uniform("uLightColor"), 2, glm::value_ptr(LightColor[0]));
	glUniform4fv(this->shaders["phong"]->uniform("uAmbient"), 1, glm::value_ptr(AmbientColor));

	// Assign to phong_bump shader
	glUseProgram(this->shaders["phong_bump"]->p());
	glUniform3fv(this->shaders["phong_bump"]->uniform("uLightPos"), 2, glm::value_ptr(LightPos[0]));
	glUniform4fv(this->shaders["phong_bump"]->uniform("uLightColor"), 2, glm::value_ptr(LightColor[0]));
	glUniform4fv(this->shaders["phong_bump"]->uniform("uAmbient"), 1, glm::value_ptr(AmbientColor));

	// And terrain
	glUseProgram(this->shaders["terrain"]->p());
	glUniform3fv(this->shaders["terrain"]->uniform("uLightPos"), 2, glm::value_ptr(LightPos[0]));
	glUniform4fv(this->shaders["terrain"]->uniform("uLightColor"), 2, glm::value_ptr(LightColor[0]));
	glUniform4fv(this->shaders["terrain"]->uniform("uAmbient"), 1, glm::value_ptr(AmbientColor));

	CHECK_OPENGL_ERROR;
}


/**
* Clear memory we allocated
**/
void RenderOpenGL::postGame()
{
	delete(this->waterobj);
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
attribute vec2 vTexUV;      \n\
varying vec2 fTexUV;        \n\
uniform mat4 uMVP;          \n\
void main() {               \n\
	gl_Position = uMVP * vec4(vPosition, 1.0); fTexUV = vTexUV;   \n\
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
	if (! this->shaders.count("basic")) {
		GLShader *shader = createProgram(pVS, pFS, "basic");
		if (shader == NULL) {
			reportFatalError("Error loading default OpenGL shader");
		}
		this->shaders["basic"] = shader;
	}
	
	// No mod loaded yet, can't load the shaders
	if (GEng()->mm == NULL) return;
	
	base = GEng()->mm->getBase();
	
	this->shaders["entities"] = loadProgram(base, "entities");
	this->shaders["bones"] = loadProgram(base, "bones");
	this->shaders["phong"] = loadProgram(base, "phong");
	this->shaders["phong_bump"] = loadProgram(base, "phong_bump");
	this->shaders["water"] = loadProgram(base, "water");
	this->shaders["terrain"] = loadProgram(base, "terrain");
	this->shaders["dissolve"] = loadProgram(base, "dissolve");
	this->shaders["text"] = loadProgram(base, "text");
	this->shaders["skybox"] = loadProgram(base, "skybox");
	
	this->shaders_loaded = true;
}


/**
* Force a reload of the shaders
**/
bool RenderOpenGL::reloadShaders()
{
	map<string, GLShader*> old = map<string, GLShader*>(this->shaders);
	
	// Load new
	this->shaders.clear();
	this->shaders_loaded = false;
	this->loadShaders();
	
	// If error, revert to old
	if (this->shaders_error) {
		for (map<string, GLShader*>::iterator it = old.begin(); it != old.end(); ++it) {
			this->shaders[it->first] = it->second;
		}
		return false;
	}
	
	// Kill off the old shaders
	for (map<string, GLShader*>::iterator it = old.begin(); it != old.end(); ++it) {
		this->deleteProgram(it->second);
	}
	
	return true;
}


/**
* Creates and compile a shader
* Returns the shader id.
**/
GLuint RenderOpenGL::createShader(const char* code, GLenum type)
{
	GLint success;
	
	GLuint shader = glCreateShader(type);
	if (shader == 0) {
		return 0;
	}
	
	#ifdef GLES
		char const *extra = "precision mediump float;";
		char *srcmod = (char*) malloc(strlen(code) + strlen(extra) + 1);
		srcmod[0] = '\0';
		strcat(srcmod, extra);
		strcat(srcmod, code);
		GLint len = strlen(srcmod);
		glShaderSource(shader, 1, (const GLchar**) &srcmod, &len);
		free(srcmod);

	#else
		GLint len = strlen(code);
		glShaderSource(shader, 1, &code, &len);
	#endif

	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (! success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(shader, 1024, NULL, InfoLog);
		GL_LOG("Error compiling shader:\n%s", InfoLog);
		return 0;
	}
	
	return shader;
}


/**
* Creates and compile a shader program from two shader code strings
* Returns the program id.
**/
GLShader* RenderOpenGL::createProgram(const char* vertex, const char* fragment, string name)
{
	GLint success;
	GLuint sVertex, sFragment;

	// Create program object
	GLuint program = glCreateProgram();
	if (program == 0) {
		return NULL;
	}
	
	// Create and attach vertex shader
	sVertex = this->createShader(vertex, GL_VERTEX_SHADER);
	if (sVertex == 0) {
		GL_LOG("Invalid vertex shader: %s", name.c_str());
		return NULL;
	}
	glAttachShader(program, sVertex);
	
	// Same with frag shader
	sFragment = this->createShader(fragment, GL_FRAGMENT_SHADER);
	if (sFragment == 0) {
		GL_LOG("Invalid fragment shader: %s", name.c_str());
		return NULL;
	}
	glAttachShader(program, sFragment);
	
	// Bind attribs
	glBindAttribLocation(program, ATTRIB_POSITION, "vPosition");
	glBindAttribLocation(program, ATTRIB_NORMAL, "vNormal");
	glBindAttribLocation(program, ATTRIB_TEXUV, "vTexUV");
	glBindAttribLocation(program, ATTRIB_BONEID, "vBoneIDs");
	glBindAttribLocation(program, ATTRIB_BONEWEIGHT, "vBoneWeights");
	glBindAttribLocation(program, ATTRIB_TEXTCOORD, "vCoord");
	glBindAttribLocation(program, ATTRIB_COLOR, "vColor");
	glBindAttribLocation(program, ATTRIB_TANGENT, "vTangent");
	
	// Link
	glLinkProgram(program);
	glDeleteShader(sVertex);
	glDeleteShader(sFragment);
	
	// Check link worked
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (! success) {
		GLchar infolog[1024];
		glGetProgramInfoLog(program, 1024, NULL, infolog);
		GL_LOG("Error linking program:\n%s", infolog);
		return NULL;
	}
	
	// Validate
	glValidateProgram(program);
	glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
	if (! success) {
		GL_LOG("Program didn't validate: %s", name.c_str());
		return NULL;
	}
	
	CHECK_OPENGL_ERROR;
	
	return new GLShader(program);
}


/**
* Load shaders using source found in a mod
*
* Will load the files:
*   shaders/<name>.glslv (vertex)
*   shaders/<name>.glslf (fragment)
**/
GLShader* RenderOpenGL::loadProgram(Mod* mod, string name)
{
	char* v;
	char* f;
	GLShader* s;
	
	#if defined(OpenGL)
		v = mod->loadText("shaders_gl/" + name + ".glslv");
		f = mod->loadText("shaders_gl/" + name + ".glslf");
	#elif defined(GLES)
		v = mod->loadText("shaders_es/" + name + ".glslv");
		f = mod->loadText("shaders_es/" + name + ".glslf");
	#endif
	
	if (v == NULL || f == NULL) {
		free(v);
		free(f);
		GL_LOG("Unable to load shader program %s", name.c_str());
		this->shaders_error = true;
		return NULL;
	}
	
	CHECK_OPENGL_ERROR;
	
	s = this->createProgram(v, f, name);
	
	CHECK_OPENGL_ERROR;
	
	free(v);
	free(f);
	
	if (s == NULL) {
		GL_LOG("Unable to create shader program %s", name.c_str());
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
void RenderOpenGL::createVBO (WavefrontObj * obj)
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
* Call this before VBO render.
* It's actually a noop in this renderer.
**/
void RenderOpenGL::preVBOrender()
{
}


/**
* Call this after VBO render, to clean up the OpenGL state.
* It's actually a noop in this renderer.
**/
void RenderOpenGL::postVBOrender()
{
}


/**
* Renders a WavefrontObj.
* This is only use by external callers (i.e. `Menu`).
* Other parts of this class just do these bits themselves, but slightly differently each time.
* Uses VBOs, so you gotta call preVBOrender() beforehand, and postVBOrender() afterwards.
**/
void RenderOpenGL::renderObj(WavefrontObj * obj, glm::mat4 mvp)
{
	GLShader *shader;
	if (obj->count == 0) this->createVBO(obj);
	
	shader = this->shaders["basic"];
	glUseProgram(shader->p());
	
	glUniformMatrix4fv(shader->uniform("uMVP"), 1, GL_FALSE, glm::value_ptr(mvp));
	
	obj->vao->bind();
	glDrawArrays(GL_TRIANGLES, 0, obj->count);
	
	CHECK_OPENGL_ERROR;
}


/**
* Renders an animation.
* Uses VBOs, so you gotta call preVBOrender() beforehand, and postVBOrender() afterwards.
*
* TODO: This needs HEAPS more work with the new animation system
**/
void RenderOpenGL::renderAnimPlay(AnimPlay * play, Entity * e)
{
	AssimpModel* am;
	GLShader* shader;

	am = play->getModel();
	if (am == NULL) return;
	
	btTransform trans = e->getTransform();
	float m[16];
	trans.getOpenGLMatrix(m);
	glm::mat4 modelMatrix = glm::make_mat4(m);


	CHECK_OPENGL_ERROR;

	// Re-calc animation if needed
	play->calcTransforms();

	// Bones? Calculate and send through bone transforms
	if (am->meshes[0]->bones.size() > 0) {
		shader = this->shaders["bones"];
		glUseProgram(shader->p());
		
		play->calcBoneTransforms();
		glUniformMatrix4fv(shader->uniform("uBones[0]"), MAX_BONES, GL_FALSE, &play->bone_transforms[0][0][0]);
			
	} else {
		shader = this->shaders["phong"];
		glUseProgram(shader->p());
	}

	recursiveRenderAssimpModel(play, am, am->rootNode, shader, modelMatrix);

	glUseProgram(0);

	CHECK_OPENGL_ERROR;
}



/**
* Render an Assimp model.
* It's a recursive function because Assimp models have a node tree
*
* @param AssimpModel *am The model
* @param AssimpNode *nd The root node of the model
* @param GLuint shader The bound shader, so uniforms will work
* @param glm::mat4 transform The node transform matrix
**/
void RenderOpenGL::recursiveRenderAssimpModel(AnimPlay* ap, AssimpModel* am, AssimpNode* nd, GLShader* shader, glm::mat4 xform_global)
{
	glm::mat4 transform = ap->getNodeTransform(nd);

	CHECK_OPENGL_ERROR;
	
	glm::mat4 MVPstatic = this->projection * this->view * xform_global * transform;
	glm::mat4 MVPbones = this->projection * this->view * xform_global;
	
	for (vector<unsigned int>::iterator it = nd->meshes.begin(); it != nd->meshes.end(); it++) {
		AssimpMesh* mesh = am->meshes[(*it)];
		
		if (mesh->bones.size() == 0) {
			glUniformMatrix4fv(shader->uniform("uMVP"), 1, GL_FALSE, glm::value_ptr(MVPstatic));
		} else {
			glUniformMatrix4fv(shader->uniform("uMVP"), 1, GL_FALSE, glm::value_ptr(MVPbones));
		}
		
		// TODO: Do we need uMV and uN for phong shading?
		
		if (am->materials[mesh->materialIndex]->diffuse == NULL) {
			glBindTexture(GL_TEXTURE_2D, 0);
		} else {
			glBindTexture(GL_TEXTURE_2D, am->materials[mesh->materialIndex]->diffuse->pixels);
		}
		
		mesh->vao->bind();
		glDrawElements(GL_TRIANGLES, mesh->numFaces*3, GL_UNSIGNED_SHORT, 0);
	}
	
	for (vector<AssimpNode*>::iterator it = nd->children.begin(); it != nd->children.end(); it++) {
		recursiveRenderAssimpModel(ap, am, (*it), shader, xform_global);
	}
	
	CHECK_OPENGL_ERROR;
}


/**
* Draws text
*
* Note that the Y is for the baseline of the text.
**/
void RenderOpenGL::renderText(string text, float x, float y, float r, float g, float b, float a)
{
	CHECK_OPENGL_ERROR;
	
	if (face == NULL) return;
	
	if (font_vbo == 0) {
		glGenBuffers(1, &font_vbo);
	}
	
	#ifdef OpenGL
		glBindVertexArray(0);
	#endif
	
	glEnable(GL_BLEND);
	
	GLShader* shader = this->shaders["text"];
	
	glUseProgram(shader->p());
	glUniform1i(shader->uniform("uTex"), 0);
	glUniform4f(shader->uniform("uColor"), r, g, b, a);
	glUniformMatrix4fv(shader->uniform("uMVP"), 1, GL_FALSE, glm::value_ptr(this->ortho));

	for (unsigned int n = 0; n < text.length(); n++ ) {
		this->renderCharacter(text[n], x, y);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);
	glDisable(GL_BLEND);
	
	CHECK_OPENGL_ERROR;
}


/**
* Draws a single character of text
* Called by ::renderText; you probably want that function instead
**/
void RenderOpenGL::renderCharacter(char character, float &x, float &y)
{
	if ((int) character < 32 || (int) character > 128) return;
	
	FreetypeChar *c = &(this->char_tex[(int) character - 32]);
	
	// If the OpenGL tex does not exist for this character, create it
	if (c->tex == 0) {
		FT_GlyphSlot slot = face->glyph;

		int error = FT_Load_Char(this->face, character, FT_LOAD_DEFAULT);
		if (error) return;

		error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
		if (error) return;
		
		int width = next_pot(slot->bitmap.width);
		int height = next_pot(slot->bitmap.rows);
		
		GLubyte* gl_data = new GLubyte[2 * width * height];
		
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
			
				gl_data[2*(i+j*width)] = gl_data[2*(i+j*width)+1] = 
					(i>=slot->bitmap.width || j>=slot->bitmap.rows) ?
					0 : slot->bitmap.buffer[i + slot->bitmap.width*j];
			
			}
		}
		
		// Create a texture
		glGenTextures(1, &c->tex);
		glBindTexture(GL_TEXTURE_2D, c->tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		#ifdef OpenGL
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		#endif
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, gl_data);
		
		delete [] gl_data;
		
		c->w = slot->bitmap.width;
		c->h = slot->bitmap.rows;
		c->x = slot->bitmap_left;
		c->y = slot->bitmap_top;
		c->advance = slot->advance.x;
		c->tx = (float)slot->bitmap.width / (float)width;
		c->ty = (float)slot->bitmap.rows / (float)height;
		
	} else {
		glBindTexture(GL_TEXTURE_2D, c->tex);
	}
	
	GLfloat box[4][4] = {
		{x + c->x,         y + -c->y + c->h,  0.f,    c->ty},
		{x + c->x + c->w,  y + -c->y + c->h,  c->tx,  c->ty},
		{x + c->x,         y + -c->y,         0.f,    0.f},
		{x + c->x + c->w,  y + -c->y,         c->tx,  0.f},
	};
	
	glBindBuffer(GL_ARRAY_BUFFER, font_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(ATTRIB_TEXTCOORD, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(ATTRIB_TEXTCOORD);
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	x += c->advance >> 6;
}


/**
* Returns the width of a string
**/
unsigned int RenderOpenGL::widthText(string text)
{
	if (face == NULL) return 0;

	unsigned int w = 0;

	for ( unsigned int n = 0; n < text.length(); n++ ) {
		char character = text[n];
		if ((int) character < 32 || (int) character > 128) continue;
		
		FT_GlyphSlot slot = face->glyph;
		
		// Load glyph image into the slot
		int error = FT_Load_Char(this->face, character, FT_LOAD_DEFAULT);
		if (error) continue;
		
		w += (slot->advance.x >> 6);
	}
	
	return w;
}



/**
* This is the main render function.
* It calls all of the sub-render parts which are defined below.
**/
void RenderOpenGL::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	CHECK_OPENGL_ERROR;

	entitiesShadowMap();

	for (unsigned int i = 0; i < this->st->num_local; i++) {
		this->render_player = this->st->local_players[i]->p;
		
		this->mainViewport(i, this->st->num_local);
		
		mainRot();
		terrain();
		entities();
		skybox();
		particles();
		water();

		if (physicsdebug != NULL) physics();
		
		this->st->local_players[i]->hud->draw();
	}

	if (this->st->num_local != 1) {
		this->mainViewport(0, 1);
	}

	guichan();
	if (this->speeddebug) fps();

	CHECK_OPENGL_ERROR;

	SDL_GL_SwapWindow(this->window);
}


/**
* Render debugging goodness for Bullet Physics
**/
void RenderOpenGL::physics()
{
#ifdef OpenGL
	CHECK_OPENGL_ERROR;
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	
	glLoadIdentity();
	glm::mat4 MVP = this->projection * this->view;
	glMultMatrixf(glm::value_ptr(MVP));
	
	st->physics->getWorld()->debugDrawWorld();

	for (list<DebugLine*>::iterator it = st->lines.begin(); it != st->lines.end(); it++) {
		glBegin(GL_LINES);
			glColor3f(1.f, 0.5f, 0.f);
			glVertex3d((*it)->a->getX(), (*it)->a->getY(), (*it)->a->getZ());
			glColor3f(1.f, 0.0f, 0.f);
			glVertex3d((*it)->b->getX(), (*it)->b->getY(), (*it)->b->getZ());
		glEnd();
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	
	CHECK_OPENGL_ERROR;
#endif
}


/**
* Main rotation for camera
**/
void RenderOpenGL::mainRot()
{
	CHECK_OPENGL_ERROR;
	
	btTransform trans;
	float tilt, angle, dist, lift;		// Up/down; left/right; distance of camera, dist off ground

	glEnable(GL_DEPTH_TEST);
	
	if (this->render_player == NULL) {
		trans = btTransform(btQuaternion(0,0,0,0),btVector3(st->map->width/2.0f, 0.0f, st->map->height/2.0f));
		tilt = 22.0f;
		dist = 80.0f;
		lift = 0.0f;
		angle = st->game_time / 100.0;

	} else {
		if (this->viewmode == 1) {
			tilt = 70.0f;
			dist = 50.0f;
			lift = 0.0f;
		} else if (this->viewmode == 2) {
			tilt = 10.0f;
			dist = 0.0f;
			lift = 1.72f;
		} else {
			tilt = 17.0f;
			dist = 25.0f;
			lift = 0.0f;
		}
		
		// Load the character details into the variables
		if (this->render_player->drive) {
			trans = this->render_player->drive->getTransform();
			angle = RAD_TO_DEG(PhysicsBullet::QuaternionToYaw(trans.getRotation())) + 180.0f;
			
			if (this->render_player->drive->vt->hasNode(VEHICLE_NODE_HORIZ)) {
				angle += this->render_player->mouse_angle;
			}
			
		} else {
			trans = this->render_player->getTransform();
			angle = this->render_player->mouse_angle + 180.0f;
			//tilt += this->render_player->vertical_angle;
		}
	}
	
	// Clamp angle
	while (angle < 0.0f) angle += 360.0f;
	while (angle > 360.0f) angle -= 360.0f;
	
	// Camera angle calculations
	float camerax = dist * sin(DEG_TO_RAD(angle)) * cos(DEG_TO_RAD(tilt)) + trans.getOrigin().x();
	float cameray = dist * sin(DEG_TO_RAD(tilt)) + trans.getOrigin().y() + lift;
	float cameraz = dist * cos(DEG_TO_RAD(angle)) * cos(DEG_TO_RAD(tilt)) + trans.getOrigin().z();
	
	// Prep the view matrix
	this->view = glm::mat4(1.0f);
	this->view = glm::rotate(this->view, tilt, glm::vec3(1.0f, 0.0f, 0.0f));
	this->view = glm::rotate(this->view, 360.0f - angle, glm::vec3(0.0f, 1.0f, 0.0f));
	this->view = glm::translate(this->view, glm::vec3(-camerax, -cameray, -cameraz));

	CHECK_OPENGL_ERROR;
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
	
	GLShader* s = this->shaders["skybox"];
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, st->map->skybox->pixels);
	glUseProgram(s->p());
	glCullFace(GL_FRONT);
	
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(st->map->skybox_size.x*0.5f, st->map->skybox_size.y*0.5f, st->map->skybox_size.z*0.5f)),
	modelMatrix = glm::scale(modelMatrix, st->map->skybox_size);
	modelMatrix = glm::rotate(modelMatrix, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	
	glm::mat4 MVP = this->projection * this->view * modelMatrix;
	glUniformMatrix4fv(s->uniform("uMVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	
	skybox_vao->bind();
	glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_INT, 0);
	
	glCullFace(GL_BACK);
	glUseProgram(0);
}


/**
* Terrain heightmap
**/
void RenderOpenGL::terrain()
{
	CHECK_OPENGL_ERROR;
	
	GLShader* s = this->shaders["terrain"];

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->shadow_depth_tex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, st->map->terrain->pixels);
	
	glUseProgram(s->p());

	glm::mat4 modelMatrix = glm::scale(
		glm::mat4(1.0f),
		glm::vec3(this->st->map->heightmapScaleX(), 1.0f, this->st->map->heightmapScaleZ())
	);
	
	glm::mat4 MVP = this->projection * this->view * modelMatrix;
	glUniformMatrix4fv(s->uniform("uMVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	
	glm::mat4 MV = this->view * modelMatrix;
	glUniformMatrix4fv(s->uniform("uMV"), 1, GL_FALSE, glm::value_ptr(MV));
	
	glm::mat3 N = glm::inverseTranspose(glm::mat3(MV));
	glUniformMatrix3fv(s->uniform("uN"), 1, GL_FALSE, glm::value_ptr(N));
	
	glm::mat4 biasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
	);

	glm::mat4 depthBiasMVP = biasMatrix * this->depthmvp * modelMatrix;
	glUniformMatrix4fv(s->uniform("uDepthBiasMVP"), 1, GL_FALSE, glm::value_ptr(depthBiasMVP));

	glUniform1i(s->uniform("uShadowMap"), 1);

	this->ter_vao->bind();

	int numPerStrip = 2 + ((st->map->heightmap_sx-1) * 2);
	for (int z = 0; z < st->map->heightmap_sz - 1; z++) {
		glDrawArrays(GL_TRIANGLE_STRIP, numPerStrip * z, numPerStrip);
	}

	
	// Static geometry meshes
	for (vector<MapMesh*>::iterator it = st->map->meshes.begin(); it != st->map->meshes.end(); it++) {
		MapMesh* mm = (*it);

		float m[16];
		mm->xform.getOpenGLMatrix(m);
		glm::mat4 modelMatrix = glm::make_mat4(m);

		recursiveRenderAssimpModel(mm->play, mm->model, mm->model->rootNode, s, modelMatrix);
	}

	glUseProgram(0);
	
	CHECK_OPENGL_ERROR;
}


/**
* Water goes last because of we do fun things with blending
**/
void RenderOpenGL::water()
{
	if (! this->st->map->water) return;

	CHECK_OPENGL_ERROR;

	glEnable(GL_BLEND);

	glBindTexture(GL_TEXTURE_2D, this->st->map->water->pixels);
	glUseProgram(this->shaders["water"]->p());
		
	if (this->waterobj->count == 0) this->createVBO(this->waterobj);
	
	glm::mat4 modelMatrix = glm::scale(
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, this->st->map->water_level, 0.0f)),
		glm::vec3(this->st->map->width, 1.0f, this->st->map->height)
	);

	glm::mat4 MVP = this->projection * this->view * modelMatrix;
	glUniformMatrix4fv(this->shaders["water"]->uniform("uMVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	
	this->waterobj->vao->bind();
	glDrawArrays(GL_TRIANGLES, 0, this->waterobj->count);

	glUseProgram(0);

	glDisable(GL_BLEND);
	
	CHECK_OPENGL_ERROR;
}


/**
* The entities are basically just AnimPlay objects
**/
void RenderOpenGL::entities()
{
	CHECK_OPENGL_ERROR;

	glEnable(GL_BLEND);

	for (list<Entity*>::iterator it = st->entities.begin(); it != st->entities.end(); it++) {
		Entity *e = (*it);
		
		if (this->viewmode == 2 && e == this->render_player) continue;
		if (e->visible == false) continue;

		AnimPlay *play = e->getAnimModel();
		if (play == NULL) continue;

		renderAnimPlay(play, e);
	}

	glDisable(GL_BLEND);

	CHECK_OPENGL_ERROR;
}


/**
* Particle effects using SPARK
**/
void RenderOpenGL::particles()
{
	#ifdef USE_SPARK
		glEnable(GL_BLEND);
		
		glm::mat4 vp = this->projection * this->view;
		this->particle_renderer->setVP(vp);
		
		this->st->particle_system->render();
		
		CHECK_OPENGL_ERROR;
	#endif
}


/**
* Guichan dialogs
**/
void RenderOpenGL::guichan()
{
	if (! GEng()->hasDialogs()) return;

	CHECK_OPENGL_ERROR;
	
	glDisable(GL_DEPTH_TEST);
	
	GEng()->gui->draw();
	
	CHECK_OPENGL_ERROR;
}


/**
* FPS counter - a testing option
**/
void RenderOpenGL::fps()
{
	char buf[50];
	float tick = GEng()->getAveTick();
	
	sprintf(buf, "%.2f ms", tick);
	this->renderText(buf, 400.0f, 50.0f);
	
	sprintf(buf, "%.1f fps", 1000.0f/tick);
	this->renderText(buf, 550.0f, 50.0f);
}


