// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <algorithm>
#include <map>

#include <GL/glew.h>
#include <GL/gl.h>
#if defined(__WIN32__)
	#include <GL/glext.h>
#endif
#include <GL/glu.h>

#include <SDL_image.h>
#include <math.h>

#include "../rage.h"
#include "../gamestate.h"
#include "../entity/player.h"
#include "../util/obj.h"
#include "../util/sdl_util.h"
#include "../util/SDL_rwops_zzip.h"
#include "../fx/newparticle.h"
#include "../mod/mod_manager.h"
#include "../mod/vehicletype.h"
#include "render_opengl.h"
#include "render_opengl_settings.h"
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

#include <ft2build.h>
#include FT_FREETYPE_H

using namespace std;


/**
* For VBO pointer offsets
**/
#define BUFFER_OFFSET(i) ((char *)NULL + (i))


/**
* For reporting errors - but only in debug mode
**/
#ifdef RELEASE
	#define CHECK_OPENGL_ERROR
#else
	#define CHECK_OPENGL_ERROR \
	{	GLenum error; \
		error = glGetError(); \
		if (error != GL_NO_ERROR) { \
			cerr << "OpenGL Error:\n"; \
			while (error) { \
				cerr << " - " << gluErrorString(error) << "\n"; \
				error = glGetError(); \
			} \
			cerr << "Location: " << __FILE__ << ":" << __LINE__ << "\n"; \
			reportFatalError("OpenGL error!"); \
		} \
	}
#endif



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
	this->screen = NULL;
	this->physicsdebug = NULL;
	this->speeddebug = false;
	this->viewmode = 0;
	this->face = NULL;
	this->particle_vao = 0;
	this->font_vbo = 0;
	
	const SDL_VideoInfo* mode = SDL_GetVideoInfo();
	this->desktop_width = mode->current_w;
	this->desktop_height = mode->current_h;
	
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
	
	this->real_width = width;
	this->real_height = height;
	
	
	// SDL
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	if (this->settings->msaa >= 2) {
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, this->settings->msaa);
	} else {
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
	}

	flags = SDL_OPENGL;
	if (fullscreen) flags |= SDL_FULLSCREEN;
	
	this->screen = SDL_SetVideoMode(width, height, 32, flags);
	if (screen == NULL) {
		char buffer[200];
		sprintf(buffer, "Unable to set %ix%i video: %s\n", width, height, SDL_GetError());
		reportFatalError(buffer);
	}
	
	// Window title
	char buff[100];
	sprintf(buff, "Chaotic Rage %s", VERSION);
	SDL_WM_SetCaption(buff, buff);

	// SDL_Image
	flags = IMG_INIT_PNG;
	int initted = IMG_Init(flags);
	if ((initted & flags) != flags) {
		reportFatalError("Failed to init required png support.");
	}
	
	// OpenGL
	if (atof((char*) glGetString(GL_VERSION)) < 3.1) {
		reportFatalError("OpenGL 3.1 or later is required for the high-end renderer, but not supported on this system. Try the low-end one.");
	}
	
	// Glew
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "Glew Error: %s\n", glewGetErrorString(err));
		reportFatalError("Unable to init the library GLEW.");
	}
	
	// GL_ARB_framebuffer_object -> glGenerateMipmap
	if (! GL_ARB_framebuffer_object) {
		reportFatalError("OpenGL 3.0 or the extension 'GL_ARB_framebuffer_object' not available.");
	}
	
	// Freetype
	int error;
	error = FT_Init_FreeType(&this->ft);
	if (error) {
		reportFatalError("Freetype: Unable to init library.");
	}
	
	// Re-load textures
	if (loaded.size() > 0) {
		for (unsigned int i = 0; i != loaded.size(); i++) {
			this->surfaceToOpenGL(loaded.at(i));
		}
	}
	
	// Re-load FreeType character textures
	for (unsigned int i = 0; i < NUM_CHAR_TEX; i++) {
		if (this->char_tex[i].tex) {
			glDeleteTextures(1, &this->char_tex[i].tex);
			this->char_tex[i].tex = 0;
		}
	}
	
	this->loadShaders();
	if (this->shaders_error) {
		reportFatalError("Error loading OpenGL shaders");
	}
	
	// OpenGL env
	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFlush();
	SDL_GL_SwapBuffers();
}


/**
* Load a font using freetype
**/
void RenderOpenGL::loadFont(string name, Mod * mod)
{
	int error, len;
	Uint8 * buf = mod->loadBinary(name, &len);

	error = FT_New_Memory_Face(this->ft, (const FT_Byte *) buf, len, 0, &this->face);
	if (error == FT_Err_Unknown_File_Format) {
		fprintf(stderr, "Freetype: Unsupported font format\n");
		exit(1);
		
	} else if (error) {
		fprintf(stderr, "Freetype: Unable to load font\n");
		exit(1);
	}
	
	error = FT_Set_Char_Size(this->face, 0, 20*64, 72, 72);
	if (error) {
		fprintf(stderr, "Freetype: Unable to load font size\n");
		exit(1);
	}

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
	
	this->projection = glm::perspective(45.0f, (float)this->virt_width / (float)this->virt_height, 1.0f, 150.0f);

	this->ortho = glm::ortho<float>(0.0f, (float)this->virt_width, (float)this->virt_height, 0.0f, -1.0f, 1.0f);
}


/**
* Enable physics debug drawing
**/
void RenderOpenGL::setPhysicsDebug(bool status)
{
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
		fprintf(stderr, "Couldn't load sprite '%s'\n", filename.c_str());
		load_err = true;
		return NULL;
	}
	
	// Checks
	if ((surf->w & (surf->w - 1)) != 0) {
		DEBUG("vid", "Bitmap '%s' width is not a power of 2", filename.c_str());
	}
	
	if ((surf->h & (surf->h - 1)) != 0) {
		DEBUG("vid", "Bitmap '%s' height is not a power of 2", filename.c_str());
	}
	
	if (surf->format->BytesPerPixel != 4 && surf->format->BytesPerPixel != 3) {
		fprintf(stderr, "Bitmap '%s' not in 32-bit or 24-bit colour; unable to load into OpenGL\n", filename.c_str());
		load_err = true;
		return NULL;
	}

	// Create the sprite object
	SpritePtr sprite = new struct sprite();
	sprite->w = surf->w;
	sprite->h = surf->h;
	sprite->orig = surf;
	
	this->surfaceToOpenGL(sprite);
	
	loaded.push_back(sprite);

	return sprite;
}


/**
* Loads an SDL_Surface into an OpenGL texture
**/
void RenderOpenGL::surfaceToOpenGL(SpritePtr sprite)
{
	GLenum texture_format;
	GLint num_colors;

	// Determine OpenGL import type
	num_colors = sprite->orig->format->BytesPerPixel;
	if (num_colors == 4) {
		if (sprite->orig->format->Rmask == 0x000000ff) {
			texture_format = GL_RGBA;
		} else {
			texture_format = GL_BGRA;
		}
		
	} else if (num_colors == 3) {
		if (sprite->orig->format->Rmask == 0x000000ff) {
			texture_format = GL_RGB;
		} else {
			texture_format = GL_BGR;
		}	
	}

	// Open texture handle
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &sprite->pixels);
	glBindTexture(GL_TEXTURE_2D, sprite->pixels);
	
	// Set stretching properties
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->mag_filter);

	// Load it
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sprite->orig->w, sprite->orig->h, 0, texture_format, GL_UNSIGNED_BYTE, sprite->orig->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
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
	
	cubemap = new struct sprite();
	
	glGenTextures(1, &cubemap->pixels);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->pixels);
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, this->min_filter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, this->mag_filter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
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
				texture_format = GL_BGRA;
			}
		
		} else if (surf->format->BytesPerPixel == 3) {
			if (surf->format->Rmask == 0x000000ff) {
				texture_format = GL_RGB;
			} else {
				texture_format = GL_BGR;
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
	glGenVertexArrays(1, &this->ter_vaoid);
	glBindVertexArray(this->ter_vaoid);
	
	// Create interleaved VBO
	glGenBuffers(1, &this->ter_vboid);
	glBindBuffer(GL_ARRAY_BUFFER, this->ter_vboid);
	
	// Set data
	glBufferData(GL_ARRAY_BUFFER, sizeof(VBOvertex) * this->ter_size, vertexes, GL_STATIC_DRAW);
	
	// and attributes
	glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(VBOvertex), BUFFER_OFFSET(0));	// Position
	glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(VBOvertex), BUFFER_OFFSET(12));	// Normals
	glVertexAttribPointer(ATTRIB_TEXUV, 2, GL_FLOAT, GL_FALSE, sizeof(VBOvertex), BUFFER_OFFSET(24));	// TexUVs

	glEnableVertexAttribArray(ATTRIB_POSITION);
	glEnableVertexAttribArray(ATTRIB_NORMAL);
	glEnableVertexAttribArray(ATTRIB_TEXUV);
	glClientActiveTexture(GL_TEXTURE0);
	
	glBindVertexArray(0);
	
	delete [] vertexes;
}


/**
* Free a loaded heightmap.
**/
void RenderOpenGL::freeHeightmap()
{
	glDeleteBuffers(1, &this->ter_vboid);
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
	glGenVertexArrays(1, &this->skybox_vaoid);
	glBindVertexArray(this->skybox_vaoid);
	
	// Position
	glGenBuffers(1, &vertex);
	glBindBuffer(GL_ARRAY_BUFFER, vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(ATTRIB_POSITION);
	glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	// Index
	glGenBuffers(1, &index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);
	
	glBindVertexArray(0);
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
	glBindTexture(GL_TEXTURE_2D, sprite->pixels);
 	
	glBegin(GL_QUADS);
		// Bottom-left vertex (corner)
		glTexCoord2i( 0, 1 );
		glVertex2i( x, y + h );
		
		// Bottom-right vertex (corner)
		glTexCoord2i( 1, 1 );
		glVertex2i( x + w, y + h );
		
		// Top-right vertex (corner)
		glTexCoord2i( 1, 0 );
		glVertex2i( x + w, y );
		
		// Top-left vertex (corner)
		glTexCoord2i( 0, 0 );
		glVertex2i( x, y );
	glEnd();
}


/**
* Various bits of set up before a game begins
* Some state variables get clobbered by the menu, so we have to set them here instead.
**/
void RenderOpenGL::preGame()
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE);
	
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	if (this->settings->msaa >= 2) {
		glEnable(GL_MULTISAMPLE);
	} else {
		glDisable(GL_MULTISAMPLE);
	}
	
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
	
	// Init the viewport for single screen only once
	if (this->st->num_local == 1) {
		this->mainViewport(1, 1);
	}
	
	// For the HUD etc
	glMatrixMode(GL_PROJECTION);
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
static const char* pVS = "                                                    \n\
#version 120                                                                  \n\
attribute  vec3 vPosition;                                                    \n\
attribute  vec3 vNormal;                                                      \n\
attribute  vec2 vTexUV;                                                       \n\
varying vec2 fTexUV;                                                          \n\
void main() {                                                                 \n\
    gl_Position = gl_ModelViewProjectionMatrix * vec4(vPosition, 1.0f); fTexUV = vTexUV; \n\
}";

/**
* Basic fragment shader for use before the base mod has been loaded
* Just textures, no lighting
**/
static const char* pFS = "                                                    \n\
#version 120                                                                  \n\
varying vec2 fTexUV;                                                          \n\
uniform sampler2D uTex;                                                       \n\
void main() {                                                                 \n\
    gl_FragColor = texture2D(uTex, fTexUV);                                   \n\
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
		this->shaders["basic"] = createProgram(pVS, pFS, "basic");
	}
	
	// No mod loaded yet, can't load the shaders
	if (! this->st->mm) return;
	
	base = this->st->mm->getBase();
	
	this->shaders["entities"] = loadProgram(base, "entities");
	this->shaders["bones"] = loadProgram(base, "bones");
	this->shaders["phong"] = loadProgram(base, "phong");
	this->shaders["phong_bump"] = loadProgram(base, "phong_bump");
	this->shaders["water"] = loadProgram(base, "water");
	this->shaders["particles"] = loadProgram(base, "particles");
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
	
	GLint len = strlen(code);
	glShaderSource(shader, 1, &code, &len);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (! success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(shader, 1024, NULL, InfoLog);
		cout << "Error compiling shader:\n" << InfoLog << "\n";
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
	
	GLuint program = glCreateProgram();
	if (program == 0) {
		return NULL;
	}
	
	// Create and attach vertex shader
	sVertex = this->createShader(vertex, GL_VERTEX_SHADER);
	if (sVertex == 0) {
		return NULL;
	}
	glAttachShader(program, sVertex);
	
	// Same with frag shader
	sFragment = this->createShader(fragment, GL_FRAGMENT_SHADER);
	if (sFragment == 0) {
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
		GLchar InfoLog[1024];
		glGetProgramInfoLog(program, 1024, NULL, InfoLog);
		cout << "Error linking program:\n" << InfoLog << "\n";
		return NULL;
	}
	
	// Validate
	glValidateProgram(program);
	glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
	if (! success) {
		return NULL;
	}
	
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
	char* v = mod->loadText("shaders/" + name + ".glslv");
	char* f = mod->loadText("shaders/" + name + ".glslf");
	GLShader* s;
	
	if (v == NULL or f == NULL) {
		free(v);
		free(f);
		cout << "Unable to load shader program " << name << endl;
		this->shaders_error = true;
		return NULL;
	}
	
	CHECK_OPENGL_ERROR;
	
	s = this->createProgram(v, f, name);
	
	CHECK_OPENGL_ERROR;
	
	free(v);
	free(f);
	
	if (s == NULL) {
		cout << "Unable to create shader program " << name << endl;
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
	CHECK_OPENGL_ERROR;
	
	// Create VAO
	GLuint vaoid;
	glGenVertexArrays(1, &vaoid);
	glBindVertexArray(vaoid);

	// Create VBO; we interleave the attributess
	GLuint vboid;
	glGenBuffers(1, &vboid);
	glBindBuffer(GL_ARRAY_BUFFER, vboid);
	
	// Build the data array
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
	
	// Set data
	glBufferData(GL_ARRAY_BUFFER, sizeof(VBOvertex) * obj->faces.size() * 3, vertexes, GL_STATIC_DRAW);
	
	glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 32, BUFFER_OFFSET(0));	// Position
	glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, 32, BUFFER_OFFSET(12));	// Normals
	glVertexAttribPointer(ATTRIB_TEXUV, 2, GL_FLOAT, GL_FALSE, 32, BUFFER_OFFSET(24));	// TexUVs

	glEnableVertexAttribArray(ATTRIB_POSITION);
	glEnableVertexAttribArray(ATTRIB_NORMAL);
	glEnableVertexAttribArray(ATTRIB_TEXUV);

	glClientActiveTexture(GL_TEXTURE0);
	
	glBindVertexArray(0);
	
	// Save if of VAO, VBO, etc.
	obj->vao = vaoid;
	obj->vbo = vboid;
	obj->ibo_count = obj->faces.size() * 3;
	
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
void RenderOpenGL::renderObj (WavefrontObj * obj)
{
	if (obj->ibo_count == 0) this->createVBO(obj);
	
	CHECK_OPENGL_ERROR;
	
	glBindVertexArray(obj->vao);
	glUseProgram(this->shaders["basic"]->p());

	glDrawArrays(GL_TRIANGLES, 0, obj->ibo_count);
	
	glUseProgram(0);
	glBindVertexArray(0);
	
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
	glm::mat4 transform;
	
	std::map<AssimpNode*, glm::mat4>::iterator local = ap->transforms.find(nd);
	assert(local != ap->transforms.end());
	transform = local->second;

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
		
		glBindVertexArray(mesh->vao);
		glDrawElements(GL_TRIANGLES, mesh->numFaces*3, GL_UNSIGNED_INT, 0);
	}
	
	for (vector<AssimpNode*>::iterator it = nd->children.begin(); it != nd->children.end(); it++) {
		recursiveRenderAssimpModel(ap, am, (*it), shader, xform_global);
	}
	
	CHECK_OPENGL_ERROR;

	glBindVertexArray(0);
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
	if (font_vbo == 0) glGenBuffers(1, &font_vbo);

	glEnable(GL_BLEND);
	
	glBindBuffer(GL_ARRAY_BUFFER, font_vbo);
	glEnableVertexAttribArray(ATTRIB_TEXTCOORD);
	glVertexAttribPointer(ATTRIB_TEXTCOORD, 4, GL_FLOAT, GL_FALSE, 0, 0);

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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
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

	glEnableVertexAttribArray(ATTRIB_TEXTCOORD);
	glVertexAttribPointer(ATTRIB_TEXTCOORD, 4, GL_FLOAT, GL_FALSE, 0, 0);

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

	for (unsigned int i = 0; i < this->st->num_local; i++) {
		this->render_player = this->st->local_players[i]->p;
		
		if (this->st->num_local != 1) {
			this->mainViewport(i, this->st->num_local);
		}
		
		mainRot();
		terrain();
		entities();
		skybox();
		particles();
		water();
		if (physicsdebug != NULL) physics();
		hud(this->st->local_players[i]->hud);
	}

	if (this->st->num_local != 1) {
		this->mainViewport(0, 1);
	}
	
	guichan();
	if (this->speeddebug) fps();

	CHECK_OPENGL_ERROR;

	SDL_GL_SwapBuffers();
}


/**
* Render debugging goodness for Bullet Physics
**/
void RenderOpenGL::physics()
{
	CHECK_OPENGL_ERROR;
	
	glDisable(GL_LIGHTING);
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

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	
	CHECK_OPENGL_ERROR;
}


/**
* Main rotation for camera
**/
void RenderOpenGL::mainRot()
{
	CHECK_OPENGL_ERROR;
	
	btTransform trans;
	float tilt, angle, dist, lift;		// Up/down; left/right; distance of camera, dist off ground
	static float deadang = 22.0f;

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glLoadIdentity();
	
	if (this->render_player == NULL) {
		trans = btTransform(btQuaternion(0,0,0,0),btVector3(st->map->width/2.0f, 0.0f, st->map->height/2.0f));
		tilt = 22.0f;
		dist = 80.0f;
		lift = 0.0f;
		angle = deadang;
		deadang += 0.05f;

	} else {
		if (this->viewmode == 0) {
			tilt = 17.0f;
			dist = 25.0f;
			lift = 0.0f;
		} else if (this->viewmode == 1) {
			tilt = 70.0f;
			dist = 50.0f;
			lift = 0.0f;
		} else if (this->viewmode == 2) {
			tilt = 10.0f;
			dist = 0.0f;
			lift = 1.72f;
		}
		
		// Load the character details into the variables
		if (this->render_player->drive) {
			trans = this->render_player->drive->getTransform();
			angle = RAD_TO_DEG(PhysicsBullet::QuaternionToYaw(trans.getRotation())) + 180.0f;
			
			if (this->render_player->drive->vt->horiz_move_node != "") {
				angle += this->render_player->mouse_angle;
			}
			
		} else {
			trans = this->render_player->getTransform();
			angle = this->render_player->mouse_angle + 180.0f;
			//tilt += this->render_player->vertical_angle;
		}
	}

	angle = clampAnglef(angle);
	
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
* Render the skybox
**/
void RenderOpenGL::skybox()
{
	if (st->map->skybox == NULL) return;
	
	GLShader* s = this->shaders["skybox"];
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, st->map->skybox->pixels);
	glUseProgram(s->p());
	glCullFace(GL_FRONT);
	
	glm::mat4 modelMatrix = glm::scale(
		glm::translate(glm::mat4(1.0f), glm::vec3(st->map->skybox_size.x*0.25f, st->map->skybox_size.y*0.5f, st->map->skybox_size.z*0.25f)),
		st->map->skybox_size
	);
	
	modelMatrix = glm::rotate(modelMatrix, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	
	glm::mat4 MVP = this->projection * this->view * modelMatrix;
	glUniformMatrix4fv(s->uniform("uMVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	
	glBindVertexArray(skybox_vaoid);
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
	
	GLShader* s = this->shaders["phong"];


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
	
	glBindVertexArray(this->ter_vaoid);

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

	glBindVertexArray(0);
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
		
	if (this->waterobj->ibo_count == 0) this->createVBO(this->waterobj);
	
	glm::mat4 modelMatrix = glm::scale(
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, this->st->map->water_level, 0.0f)),
		glm::vec3(this->st->map->width, 1.0f, this->st->map->height)
	);

	glm::mat4 MVP = this->projection * this->view * modelMatrix;
	glUniformMatrix4fv(this->shaders["water"]->uniform("uMVP"), 1, GL_FALSE, glm::value_ptr(MVP));
		
	glBindVertexArray(this->waterobj->vao);
	glDrawArrays(GL_TRIANGLES, 0, this->waterobj->ibo_count);

	glBindVertexArray(0);
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
		if (e->render == false) continue;

		AnimPlay *play = e->getAnimModel();
		if (play == NULL) continue;

		renderAnimPlay(play, e);
	}

	glDisable(GL_BLEND);

	CHECK_OPENGL_ERROR;
}


/**
* Particle effects
* This is a bit incomplete and sloppy at the moment
**/
void RenderOpenGL::particles()
{
	unsigned int size = this->st->particles.size();
	if (size == 0) return;

	CHECK_OPENGL_ERROR;

	// First time set up
	if (!this->particle_vao) {
		glGenVertexArrays(1,&(this->particle_vao));
		glBindVertexArray(this->particle_vao);

		glGenBuffers(1, &(this->particle_vbo));
		glBindBuffer(GL_ARRAY_BUFFER, this->particle_vbo);
		glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 24, BUFFER_OFFSET(0));		// Position
		glVertexAttribPointer(ATTRIB_COLOR, 3, GL_FLOAT, GL_FALSE, 24, BUFFER_OFFSET(12));		// Colour
		
		glEnableVertexAttribArray(ATTRIB_POSITION);
		glEnableVertexAttribArray(ATTRIB_COLOR);

		glBindVertexArray(0);
	}

	// This is a pretty horrible way to do this, because we malloc() and free() once per frame
	// but it's still better than glBegin/glEnd.
	// TODO: Save our particles in a simple ready-to-go struct, so we can skip all this
	float* data = (float*) malloc(sizeof(float) * 6 * size);
	int i = 0;
	for (list<NewParticle*>::iterator it = this->st->particles.begin(); it != this->st->particles.end(); it++) {
		data[i] = (*it)->pos.x();
		data[i+1] = (*it)->pos.y();
		data[i+2] = (*it)->pos.z();
		data[i+3] = (*it)->r;
		data[i+4] = (*it)->g;
		data[i+5] = (*it)->b;
		i += 6;
	}
	glBindBuffer(GL_ARRAY_BUFFER, this->particle_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * size, data, GL_DYNAMIC_DRAW);
	free(data);


	// Now we can draw
	glBindVertexArray(this->particle_vao);

	// Bind shader
	glUseProgram(this->shaders["particles"]->p());
	
	// Uniforms
	glm::mat4 MVP = this->projection * this->view;
	glUniformMatrix4fv(this->shaders["particles"]->uniform("uMVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	
	// Draw
	glDrawArrays(GL_POINTS, 0, size);
	
	glUseProgram(0);
	glBindVertexArray(0);
	

	CHECK_OPENGL_ERROR;
}


/**
* Guichan dialogs
**/
void RenderOpenGL::guichan()
{
	if (! st->hasDialogs()) return;

	CHECK_OPENGL_ERROR;
	
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	
	glLoadIdentity();
	this->st->gui->draw();
	
	CHECK_OPENGL_ERROR;
}


/**
* Heads-up display (ammo, health, etc)
**/
void RenderOpenGL::hud(HUD * hud)
{
	CHECK_OPENGL_ERROR;
	
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	
	glLoadIdentity();
	glOrtho(0.0, this->virt_width, this->virt_height, 0.0, -1.0, 1.0);
	hud->render(this);
	
	CHECK_OPENGL_ERROR;
}


/**
* FPS counter - a testing option
**/
void RenderOpenGL::fps()
{
	char buf[50];
	float tick = this->st->getAveTick();
	
	sprintf(buf, "%.2f ms", tick);
	this->renderText(buf, 400.0f, 50.0f);
	
	sprintf(buf, "%.1f fps", 1000.0f/tick);
	this->renderText(buf, 550.0f, 50.0f);
}










//////////////////////////////////////////////
///
///   OpenGL debug drawing class.
///   Originally from bullet/demos/opengl.
///
//////////////////////////////////////////////


GLDebugDrawer::GLDebugDrawer():m_debugMode(0)
{
}

void GLDebugDrawer::drawLine(const btVector3& from,const btVector3& to,const btVector3& fromColor, const btVector3& toColor)
{
	glBegin(GL_LINES);
		glColor3f(fromColor.getX(), fromColor.getY(), fromColor.getZ());
		glVertex3d(from.getX(), from.getY(), from.getZ());
		glColor3f(toColor.getX(), toColor.getY(), toColor.getZ());
		glVertex3d(to.getX(), to.getY(), to.getZ());
	glEnd();
}

void GLDebugDrawer::drawLine(const btVector3& from,const btVector3& to,const btVector3& color)
{
	drawLine(from,to,color,color);
}

void GLDebugDrawer::drawSphere (const btVector3& p, btScalar radius, const btVector3& color)
{
	glColor4f (color.getX(), color.getY(), color.getZ(), btScalar(1.0f));
	glPushMatrix ();
	glTranslatef (p.getX(), p.getY(), p.getZ());

	int lats = 5;
	int longs = 5;

	int i, j;
	for(i = 0; i <= lats; i++) {
		btScalar lat0 = SIMD_PI * (-btScalar(0.5) + (btScalar) (i - 1) / lats);
		btScalar z0  = radius*sin(lat0);
		btScalar zr0 =  radius*cos(lat0);

		btScalar lat1 = SIMD_PI * (-btScalar(0.5) + (btScalar) i / lats);
		btScalar z1 = radius*sin(lat1);
		btScalar zr1 = radius*cos(lat1);

		glBegin(GL_QUAD_STRIP);
		for(j = 0; j <= longs; j++) {
			btScalar lng = 2 * SIMD_PI * (btScalar) (j - 1) / longs;
			btScalar x = cos(lng);
			btScalar y = sin(lng);

			glNormal3f(x * zr0, y * zr0, z0);
			glVertex3f(x * zr0, y * zr0, z0);
			glNormal3f(x * zr1, y * zr1, z1);
			glVertex3f(x * zr1, y * zr1, z1);
		}
		glEnd();
	}

	glPopMatrix();
}

void GLDebugDrawer::drawBox (const btVector3& boxMin, const btVector3& boxMax, const btVector3& color, btScalar alpha)
{
	btVector3 halfExtent = (boxMax - boxMin) * btScalar(0.5f);
	btVector3 center = (boxMax + boxMin) * btScalar(0.5f);
	glColor4f (color.getX(), color.getY(), color.getZ(), alpha);
	glPushMatrix ();
	glTranslatef (center.getX(), center.getY(), center.getZ());
	glScaled(2*halfExtent[0], 2*halfExtent[1], 2*halfExtent[2]);
	glPopMatrix ();
}

void GLDebugDrawer::drawTriangle(const btVector3& a,const btVector3& b,const btVector3& c,const btVector3& color,btScalar alpha)
{
	const btVector3	n=btCross(b-a,c-a).normalized();
	
	glBegin(GL_TRIANGLES);
	glColor4f(color.getX(), color.getY(), color.getZ(),alpha);
	glNormal3d(n.getX(),n.getY(),n.getZ());
	glVertex3d(a.getX(),a.getY(),a.getZ());
	glVertex3d(b.getX(),b.getY(),b.getZ());
	glVertex3d(c.getX(),c.getY(),c.getZ());
	glEnd();
}

void GLDebugDrawer::setDebugMode(int debugMode)
{
	m_debugMode = debugMode;
}

void GLDebugDrawer::draw3dText(const btVector3& location,const char* textString)
{
	glRasterPos3f(location.x(),  location.y(),  location.z());
}

void GLDebugDrawer::reportErrorWarning(const char* warningString)
{
	DEBUG("phy", "%s\n", warningString);
}

void GLDebugDrawer::drawContactPoint(const btVector3& pointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color)
{
	btVector3 to = pointOnB + normalOnB * 1;//distance;
	
	const btVector3&from = pointOnB;
	
	glColor4f(color.getX(), color.getY(), color.getZ(),1.f);
	
	glBegin(GL_LINES);
	glVertex3d(from.getX(), from.getY(), from.getZ());
	glVertex3d(to.getX(), to.getY(), to.getZ());
	glEnd();
}

