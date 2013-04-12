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

#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <guichan/opengl.hpp>
#include "guichan_imageloader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/scene.h>

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
			while (error) { \
				cout << "OpenGL Error: " << gluErrorString(error) << "\n"; \
			} \
			cout << "Location: " << __FILE__ << ":" << __LINE__ << "\n"; \
			reportFatalError("OpenGL error detected; exiting."); \
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


RenderOpenGL::RenderOpenGL(GameState * st) : Render3D(st)
{
	this->screen = NULL;
	this->physicsdebug = NULL;
	this->speeddebug = false;
	this->viewmode = 0;
	this->face = NULL;
	this->particle_vao = 0;

	const SDL_VideoInfo* mode = SDL_GetVideoInfo();
	this->desktop_width = mode->current_w;
	this->desktop_height = mode->current_h;
	
	for (unsigned int i = 0; i < NUM_CHAR_TEX; i++) {
		this->char_tex[i].tex = 0;
	}
	
	q_tex = 4;
	q_alias = 4;
	q_general = 4;
	
	shaders_loaded = false;
}

RenderOpenGL::~RenderOpenGL()
{
}


/**
* Sets the screen size
**/
void RenderOpenGL::setScreenSize(int width, int height, bool fullscreen)
{
	int flags;
	
	this->real_width = width;
	this->real_height = height;
	
	
	// SDL
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	if (q_alias >= 2) {
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, q_alias);
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
	
	// OpenGL env
	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFlush();
	SDL_GL_SwapBuffers();
	
	mainViewport(1, 1);
}


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
}


/**
* Enable physics debug drawing
**/
void RenderOpenGL::setPhysicsDebug(bool status)
{
	if (status) {
		this->physicsdebug = new GLDebugDrawer();
		this->physicsdebug->setDebugMode(
			btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawAabb | btIDebugDraw::DBG_DrawContactPoints | btIDebugDraw::DBG_NoDeactivation
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
* Loads an SDL_Surface into OpenGL
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
	if (q_tex >= 2) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	// Load it
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sprite->orig->w, sprite->orig->h, 0, texture_format, GL_UNSIGNED_BYTE, sprite->orig->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
}


/**
* Loads a heightmap from the raw heightmap data (an array of floats).
* TODO: This should use TRIANGLE_STRIPS not TRIANGLES for rendering.
**/
void RenderOpenGL::loadHeightmap()
{
	int nX, nZ, nTri, j;
	float flX, flZ;
	
	if (st->curr_map->heightmap == NULL) st->curr_map->createHeightmapRaw();
	if (st->curr_map->heightmap == NULL) return;
	
	this->ter_size = st->curr_map->heightmap_sx * st->curr_map->heightmap_sz * 6;
	VBOvertex* vertexes = new VBOvertex[this->ter_size];
	
	j = 0;
	for( nZ = 0; nZ < st->curr_map->heightmap_sz - 1; nZ += 1 ) {
		for( nX = 0; nX < st->curr_map->heightmap_sx - 1; nX += 1 ) {
			
			// u = p2 - p1; v = p3 - p1
			btVector3 u = btVector3(nX + 1.0f, st->curr_map->heightmapGet(nX + 1, nZ + 1), nZ + 1.0f) - btVector3(nX, st->curr_map->heightmapGet(nX, nZ), nZ);
			btVector3 v = btVector3(nX + 1.0f, st->curr_map->heightmapGet(nX + 1, nZ), nZ) - btVector3(nX, st->curr_map->heightmapGet(nX, nZ), nZ);
			
			// calc vector
			btVector3 normal = btVector3(
				u.y() * v.z() - u.z() * v.y(),
				u.z() * v.x() - u.x() * v.z(),
				u.x() * v.y() - u.y() * v.x()
			);
			
			for( nTri = 0; nTri < 6; nTri++ ) {
				// Using This Quick Hack, Figure The X,Z Position Of The Point
				flX = (float) nX + (( nTri == 1 || nTri == 2 || nTri == 4 ) ? 1.0f : 0.0f);
				flZ = (float) nZ + (( nTri == 2 || nTri == 4 || nTri == 5 ) ? 1.0f : 0.0f);
				
		 		vertexes[j].x = flX;
				vertexes[j].y = st->curr_map->heightmapGet(flX, flZ);
				vertexes[j].z = flZ;
				vertexes[j].nx = normal.x();
				vertexes[j].ny = normal.y();
				vertexes[j].nz = normal.z();
				vertexes[j].tx = flX / st->curr_map->heightmap_sx;
				vertexes[j].ty = flZ / st->curr_map->heightmap_sz;
				j++;
			}
		}
	}
	
	glGenVertexArrays(1, &this->ter_vaoid);
	glBindVertexArray(this->ter_vaoid);
	
	glGenBuffers(1, &this->ter_vboid);
	glBindBuffer(GL_ARRAY_BUFFER, this->ter_vboid);
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(VBOvertex) * this->ter_size, vertexes, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VBOvertex), BUFFER_OFFSET(0));	// Position
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VBOvertex), BUFFER_OFFSET(12));	// Normals
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VBOvertex), BUFFER_OFFSET(24));	// TexUVs

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
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


void RenderOpenGL::preGame()
{
	SDL_ShowCursor(SDL_DISABLE);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	if (q_tex >= 2) {
		glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	} else {
		glHint(GL_GENERATE_MIPMAP_HINT, GL_FASTEST);
	}

	if (q_alias >= 2) {
		glEnable(GL_MULTISAMPLE);
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	} else {
		glDisable(GL_MULTISAMPLE);
		glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);
	}

	if (q_general >= 2) {
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glHint(GL_FOG_HINT, GL_NICEST);
	} else {
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
		glHint(GL_FOG_HINT, GL_FASTEST);
	}
	
	// Set fog properties
	if (st->curr_map->fog_density > 0.0f) {
		glFogi(GL_FOG_MODE, GL_LINEAR);
		glFogfv(GL_FOG_COLOR, st->curr_map->fog_color);
		glFogf(GL_FOG_DENSITY, st->curr_map->fog_density);
		glFogf(GL_FOG_START, 80.0f);
		glFogf(GL_FOG_END, 100.0f);
	}
	
	// This will load the shaders (from the base mod) if they aren't loaded.
	this->loadShaders();
	
	// The water object
	this->createWater();
	
	this->st->addDebugPoint(100.0f, 20.0f, 100.0f);
}

void RenderOpenGL::postGame()
{
	SDL_ShowCursor(SDL_ENABLE);
	mainViewport(1, 1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_MULTISAMPLE);

	for (unsigned int i = 0; i < 8; i++) {
		glDisable(GL_LIGHT0 + i);
	}

	GLfloat em[] = {0.0, 0.0, 0.0, 0.0};
	glMaterialfv(GL_FRONT, GL_EMISSION, em);
	
	delete(this->waterobj);
}


/**
* Clears all colour from a given pixel for a given sprite
**/
void RenderOpenGL::clearPixel(int x, int y)
{
}

/**
* Free sprite memory
**/
void RenderOpenGL::freeSprite(SpritePtr sprite)
{
	glDeleteTextures(1, &sprite->pixels);
	delete(sprite);
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
* such as object rendering, background, etc.
* Doesn't include various special-effects shaders that may be in use.
**/
void RenderOpenGL::loadShaders()
{
	Mod *base;
	
	// Don't even bother if they are already loaded
	if (this->shaders_loaded) return;
	
	// Before the mod is loaded, we need some basic shaders
	// These are hardcoded (see above)
	if (! this->st->mm) {
		this->shaders["basic"] = createProgram(pVS, pFS, "basic");
		return;
	}
	
	base = this->st->mm->getBase();
	this->shaders["entities"] = loadProgram(base, "entities");
	this->shaders["map"] = loadProgram(base, "map");
	this->shaders["water"] = loadProgram(base, "water");
	this->shaders["particles"] = loadProgram(base, "particles");
	this->shaders["dissolve"] = loadProgram(base, "dissolve");

	this->shaders_loaded = true;
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
		reportFatalError("Error creating OpenGL shader");
	}
	
	GLint len = strlen(code);
	glShaderSource(shader, 1, &code, &len);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (! success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(shader, 1024, NULL, InfoLog);
		cout << "Error compiling shader:\n" << InfoLog << "\n";
		reportFatalError("Error compiling OpenGL shader");
	}
	
	return shader;
}


/**
* Creates and compile a shader program from two shader code strings
* Returns the program id.
**/
GLuint RenderOpenGL::createProgram(const char* vertex, const char* fragment, string name)
{
	GLint success;
	GLuint sVertex, sFragment;
	
	GLuint program = glCreateProgram();
	if (program == 0) {
		reportFatalError("Error creating OpenGL program");
	}
	
	// Create and attach vertex shader
	sVertex = this->createShader(vertex, GL_VERTEX_SHADER);
	glAttachShader(program, sVertex);
	
	// Same with frag shader
	sFragment = this->createShader(fragment, GL_FRAGMENT_SHADER);
	glAttachShader(program, sFragment);
	
	// Bind attribs
	glBindAttribLocation(program, 0, "vPosition");
	glBindAttribLocation(program, 1, "vNormal");
	glBindAttribLocation(program, 2, "vTexUV");
	
	// Link
	glLinkProgram(program);
	glDeleteShader(sVertex);
	glDeleteShader(sFragment);
	
	// Check link worked
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (! success) {
		GLchar InfoLog[1024];
		glGetProgramInfoLog(program, 1024, NULL, InfoLog);
		cout << "Error compiling shader:\n" << InfoLog << "\n";
		reportFatalError("Error linking OpenGL program " + name);
	}
	
	// Validate
	glValidateProgram(program);
	glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
	if (! success) {
		reportFatalError("Error validating OpenGL program " + name);
	}
	
	return program;
}


/**
* Load shaders using source found in a mod
*
* Will load the files:
*   shaders/<name>.glslv (vertex)
*   shaders/<name>.glslf (fragment)
**/
GLuint RenderOpenGL::loadProgram(Mod* mod, string name)
{
	char* v = mod->loadText("shaders/" + name + ".glslv");
	char* f = mod->loadText("shaders/" + name + ".glslf");
	
	if (v == NULL or f == NULL) {
		reportFatalError("Unable to load shader program " + name);
	}
	
	GLuint prog = this->createProgram(v, f, name);
	
	delete(v);
	delete(f);
	
	return prog;
}


/**
* Builds a VBO for this object
**/
void RenderOpenGL::createVBO (WavefrontObj * obj)
{
	GLuint vaoid;
	glGenVertexArrays(1, &vaoid);
	glBindVertexArray(vaoid);

	GLuint vboid;
	glGenBuffers(1, &vboid);
	glBindBuffer(GL_ARRAY_BUFFER, vboid);
	
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
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(VBOvertex) * obj->faces.size() * 3, vertexes, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 32, BUFFER_OFFSET(0));	// Position
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 32, BUFFER_OFFSET(12));	// Normals
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 32, BUFFER_OFFSET(24));	// TexUVs

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glClientActiveTexture(GL_TEXTURE0);
	
	glBindVertexArray(0);
	
	obj->vao = vaoid;
	obj->vbo = vboid;
	obj->ibo_count = obj->faces.size() * 3;
	
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
	
	glBindVertexArray(obj->vao);
	glUseProgram(this->shaders["basic"]);
	
	glBindAttribLocation(this->shaders["basic"], 0, "vPosition");
	glBindAttribLocation(this->shaders["basic"], 1, "vNormal");
	glBindAttribLocation(this->shaders["basic"], 2, "vTexUV");

	glDrawArrays(GL_TRIANGLES, 0, obj->ibo_count);
	
	glUseProgram(0);
	glBindVertexArray(0);
}


/**
* Renders an animation.
* Uses VBOs, so you gotta call preVBOrender() beforehand, and postVBOrender() afterwards.
**/
void RenderOpenGL::renderAnimPlay(AnimPlay * play, Entity * e)
{
	AssimpModel* am;
	GLuint shader;

	am = play->getModel();
	if (am == NULL) return;
	
	//int frame = play->getFrame();
	
	btTransform trans = e->getTransform();
	float m[16];
	trans.getOpenGLMatrix(m);
	glm::mat4 modelMatrix = glm::make_mat4(m);


	CHECK_OPENGL_ERROR;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	/*for (unsigned int d = 0; d < model->meshframes.size(); d++) {
		if (model->meshframes[d]->frame != frame) continue;
		if (model->meshframes[d]->mesh == NULL) continue;
		if (model->meshframes[d]->texture == NULL) continue;
		
		// Dissolve texture, if required
		if (model->meshframes[d]->dissolve) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, model->meshframes[d]->dissolve->pixels);
		}

		// Regular texure
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, model->meshframes[d]->texture->pixels);
		
		glMaterialfv(GL_FRONT, GL_EMISSION, model->meshframes[d]->emission);
		
		
		glm::mat4 frameMatrix = glm::translate(modelMatrix, glm::vec3(model->meshframes[d]->px, model->meshframes[d]->py, model->meshframes[d]->pz));
		frameMatrix = glm::rotate(frameMatrix, model->meshframes[d]->rx, glm::vec3(1.0f, 0.0f, 0.0f));
		frameMatrix = glm::rotate(frameMatrix, model->meshframes[d]->ry, glm::vec3(0.0f, 1.0f, 0.0f));
		frameMatrix = glm::rotate(frameMatrix, model->meshframes[d]->rz, glm::vec3(0.0f, 0.0f, 1.0f));

		// This is only temporary until the models get changed to be Y-up instead of Z-up
		frameMatrix = glm::rotate(frameMatrix, 270.0f, glm::vec3(1.0f, 0.0f, 0.0f));

		frameMatrix = glm::scale(frameMatrix, glm::vec3(model->meshframes[d]->sx, model->meshframes[d]->sy, model->meshframes[d]->sz));

		if (model->meshframes[d]->dissolve) {
			// Dissolve shader
			shader = this->shaders["dissolve"];
			float health = 1.0f;
			if (e->klass() == UNIT) {
				health = ((Unit*)e)->getHealthPercent();
			}
			glUseProgram(shader);
			glUniform1i(glGetUniformLocation(shader, "uTex"), 0);		// tex unit 0
			glUniform1i(glGetUniformLocation(shader, "uDissolve"), 1);	// tex unit 1
			glUniform1f(glGetUniformLocation(shader, "uDeath"), 1.0f - health);

		} else {*/
			// Regular shader
			shader = this->shaders["entities"];
			glUseProgram(shader);
			glUniform1i(glGetUniformLocation(shader, "uTex"), 0);		// tex unit 0
		/*}*/
		

		glBindAttribLocation(shader, 0, "vPosition");
		glBindAttribLocation(shader, 1, "vNormal");
		glBindAttribLocation(shader, 2, "vTexUV");

		recursiveRenderAssimpModel(am, am->rootNode, shader, modelMatrix);

		glUseProgram(0);
	/*}*/

	glDisable(GL_BLEND);

	CHECK_OPENGL_ERROR;
}




/**
* This is an incomplete renderer for an assimp scene object.
* Borrowed from the assimp sample code.
* It doesn't actually work at the moment - it's using fixed function, which we don't support.
**/
void RenderOpenGL::recursiveRenderAssimpModel(AssimpModel *am, AssimpNode *nd, GLuint shader, glm::mat4 transform)
{
	transform *= nd->transform;
	
	CHECK_OPENGL_ERROR;

	glm::mat4 MVP = this->projection * this->view * transform;
	glUniformMatrix4fv(glGetUniformLocation(shader, "uMVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	
	for (vector<unsigned int>::iterator it = nd->meshes.begin(); it != nd->meshes.end(); it++) {
		AssimpMesh* mesh = am->meshes[(*it)];
		
		if (am->materials[mesh->materialIndex]->tex == NULL) {
			glBindTexture(GL_TEXTURE_2D, 0);
		} else {
			glBindTexture(GL_TEXTURE_2D, am->materials[mesh->materialIndex]->tex->pixels);
		}
		
		glBindVertexArray(mesh->vao);
		glDrawElements(GL_TRIANGLES, mesh->numFaces*3, GL_UNSIGNED_INT, 0);
	}
	
	for (vector<AssimpNode*>::iterator it = nd->children.begin(); it != nd->children.end(); it++) {
		recursiveRenderAssimpModel(am, (*it), shader, transform);
	}
	
	CHECK_OPENGL_ERROR;

	glBindVertexArray(0);
}


/**
* Load a model into VBOs etc.
*
* TODO: Does this actually belong in the AssimModel class?
**/
bool RenderOpenGL::loadAssimpModel(AssimpModel *am)
{
	const struct aiScene* sc = am->getScene();
	unsigned int n = 0;
	GLuint buffer;
	
	CHECK_OPENGL_ERROR;

	for (; n < sc->mNumMeshes; ++n) {
		const struct aiMesh* mesh = sc->mMeshes[n];
		AssimpMesh *myMesh = new AssimpMesh();
		
		unsigned int *faceArray;
		faceArray = (unsigned int *)malloc(sizeof(unsigned int) * mesh->mNumFaces * 3);
		unsigned int faceIndex = 0;

		for (unsigned int t = 0; t < mesh->mNumFaces; ++t) {
			const struct aiFace* face = &mesh->mFaces[t];
			
			memcpy(&faceArray[faceIndex], face->mIndices, 3 * sizeof(int));
			faceIndex += 3;
		}
		
		myMesh->numFaces = mesh->mNumFaces;
		myMesh->materialIndex = mesh->mMaterialIndex;
		
		// Vertex Array Object
		glGenVertexArrays(1,&(myMesh->vao));
		glBindVertexArray(myMesh->vao);
		
		// Faces
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh->mNumFaces * 3, faceArray, GL_STATIC_DRAW);
		
		// Positions
		if (mesh->HasPositions()) {
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*mesh->mNumVertices, mesh->mVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);
		}
		
		// Normals
		if (mesh->HasNormals()) {
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*mesh->mNumVertices, mesh->mNormals, GL_STATIC_DRAW);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, 0, 0, 0);
		}
		
		// UVs
		if (mesh->HasTextureCoords(0)) {
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*mesh->mNumVertices, mesh->mTextureCoords[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, 0, 0, 0);
		}
		
		glBindVertexArray(0);
		
		am->meshes.push_back(myMesh);
	}
	
	CHECK_OPENGL_ERROR;

	// Load materials. Only supports simple materials with a single texture at the moment.
	// TODO: We should save these in a list so we don't load the same stuff multiple times.
	for (n = 0; n < sc->mNumMaterials; n++) {
		const aiMaterial* pMaterial = sc->mMaterials[n];
		
		AssimpMaterial *myMat = new AssimpMaterial();
		
		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString Path;
			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) != AI_SUCCESS) continue;
			
			string p(Path.data);
			
			cout << "Assimp texture: " << p << "\n";
			
			if (p.substr(0, 2) == ".\\") p = p.substr(2, p.size() - 2);
			if (p.substr(0, 2) == "./") p = p.substr(2, p.size() - 2);
			if (p.substr(0, 2) == "//") p = p.substr(2, p.size() - 2);
			
			SpritePtr tex = this->loadSprite("models/" + p, am->mod); 
			
			myMat->tex = tex;
		}
		
		am->materials.push_back(myMat);
	}
	
	CHECK_OPENGL_ERROR;

	return true;
}


/**
* Draws text
*
* Note that the Y is for the baseline of the text.
**/
void RenderOpenGL::renderText(string text, float x, float y, float r, float g, float b, float a)
{
	if (face == NULL) return;

	glPushMatrix();
	glTranslatef(x, y, 0);
	
	unsigned int n;
	
	glColor4f(r, g, b, a);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	for ( n = 0; n < text.length(); n++ ) {
		this->renderCharacter(text[n]);
	}
	
	glDisable(GL_BLEND);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	
	glPopMatrix();
}


/**
* Draws a single character
**/
void RenderOpenGL::renderCharacter(char character)
{
	if ((int) character < 32 || (int) character > 128) return;
	
	FT_GlyphSlot slot = face->glyph;
	FreetypeChar *c = &(this->char_tex[(int) character - 32]);
	
	
	// Load glyph image into the slot
	int error = FT_Load_Char(this->face, character, FT_LOAD_DEFAULT);
	if (error) return;
	
	
	// If the OpenGL tex does not exist for this character, create it
	if (c->tex == 0) {
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
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, gl_data);
		
		delete [] gl_data;
		
		c->w = slot->bitmap.width;
		c->h = slot->bitmap.rows;
		c->x = slot->bitmap_left;
		c->y = slot->bitmap_top;
		c->tx = (float)slot->bitmap.width / (float)width;
		c->ty = (float)slot->bitmap.rows / (float)height;
	}
	
	
	// Render
	glPushMatrix();
	glTranslatef(c->x, 0.f - c->y, 0.f);
	glBindTexture(GL_TEXTURE_2D, c->tex);
	
	glBegin(GL_QUADS);
		glTexCoord2d(0.f,c->ty); glVertex2f(0.f,c->h);
		glTexCoord2d(0.f,0.f); glVertex2f(0.f,0.f);
		glTexCoord2d(c->tx,0.f); glVertex2f(c->w,0.f);
		glTexCoord2d(c->tx,c->ty); glVertex2f(c->w,c->h);
	glEnd();
	
	glPopMatrix();
	glTranslatef(slot->advance.x >> 6, 0.f, 0.f);
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
		mainViewport(i, this->st->num_local);
		
		mainRot();
		terrain();
		entities();
		particles();
		water();
		if (physicsdebug != NULL) physics();
		hud(this->st->local_players[i]->hud);
	}

	mainViewport(0,1);
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
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_FOG);
	glDisable(GL_TEXTURE_2D);
	
	glm::mat4 MVP = this->projection * this->view;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
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
	glEnable(GL_FOG);
	glEnable(GL_TEXTURE_2D);
}


/**
* Main rotation for camera
**/
void RenderOpenGL::mainRot()
{
	btTransform trans;
	float tilt, angle, dist, lift;		// Up/down; left/right; distance of camera, dist off ground
	static float deadang = 22.0f;

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	if (this->render_player == NULL) {
		trans = btTransform(btQuaternion(0,0,0,0),btVector3(st->curr_map->width/2.0f, 0.0f, st->curr_map->height/2.0f));
		tilt = 22.0f;
		dist = 80.0f;
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
			lift = 0.5f;
		}
		
		// Load the character details into the variables
		if (this->render_player->drive) {
			trans = this->render_player->drive->getTransform();
			angle = RAD_TO_DEG(PhysicsBullet::QuaternionToYaw(trans.getRotation())) + 180.0f;
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

	// Enable fog
	if (this->render_player != NULL && this->viewmode == 0 && st->curr_map->fog_density > 0.0f) {
		glEnable(GL_FOG);
	}
}


/**
* Floor
**/
void RenderOpenGL::terrain()
{
	glEnable(GL_NORMALIZE);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glBindTexture(GL_TEXTURE_2D, st->curr_map->terrain->pixels);
	
	glUseProgram(this->shaders["map"]);
	

	glm::vec3 LightPos[2];
	glm::vec4 LightColor[2];
	
	unsigned int idx = 0;
	for (unsigned int i = 0; i < st->curr_map->lights.size(); i++) {
		Light * l = st->curr_map->lights[i];

		if (l->type == 3) {
			LightPos[idx] = glm::vec3(l->x, l->y, l->z);
			LightColor[idx] = glm::vec4(l->diffuse[0], l->diffuse[1], l->diffuse[2], getRandomf(0.8f, 0.9f));
			idx++;
			if (idx == 2) break;
		}
	}
	
	glUniform3fv(glGetUniformLocation(this->shaders["map"], "uLightPos"), 2, glm::value_ptr(LightPos[0]));
	glUniform4fv(glGetUniformLocation(this->shaders["map"], "uLightColor"), 2, glm::value_ptr(LightColor[0]));


	glm::mat4 modelMatrix = glm::scale(
		glm::mat4(1.0f),
		glm::vec3(this->st->curr_map->heightmapScaleX(), 1.0f, this->st->curr_map->heightmapScaleZ())
	);
	
	glm::mat4 MVP = this->projection * this->view * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(this->shaders["map"], "uMVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	
	glm::mat4 MV = this->view * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(this->shaders["map"], "uMV"), 1, GL_FALSE, glm::value_ptr(MV));
	
	glm::mat3 N = glm::inverseTranspose(glm::mat3(MV));
	glUniformMatrix3fv(glGetUniformLocation(this->shaders["map"], "uN"), 1, GL_FALSE, glm::value_ptr(N));
	
	glBindVertexArray(this->ter_vaoid);
	glDrawArrays(GL_TRIANGLES, 0, this->ter_size);
	
	
	// Static geometry meshes
	glFrontFace(GL_CCW);
	for (vector<MapMesh*>::iterator it = st->curr_map->meshes.begin(); it != st->curr_map->meshes.end(); it++) {
		glBindTexture(GL_TEXTURE_2D, (*it)->texture->pixels);
		
		float m[16];
		(*it)->xform.getOpenGLMatrix(m);
		modelMatrix = glm::make_mat4(m);
		
		WavefrontObj * obj = (*it)->mesh;
		if (obj->ibo_count == 0) this->createVBO(obj);
		
		glm::mat4 MVP = this->projection * this->view * modelMatrix;
		glUniformMatrix4fv(glGetUniformLocation(this->shaders["map"], "uMVP"), 1, GL_FALSE, glm::value_ptr(MVP));
		
		glm::mat4 MV = this->view * modelMatrix;
		glUniformMatrix4fv(glGetUniformLocation(this->shaders["map"], "uMV"), 1, GL_FALSE, glm::value_ptr(MV));
	
		glm::mat3 N = glm::inverseTranspose(glm::mat3(MV));
		glUniformMatrix3fv(glGetUniformLocation(this->shaders["map"], "uN"), 1, GL_FALSE, glm::value_ptr(N));

		glBindVertexArray(obj->vao);
		glDrawArrays(GL_TRIANGLES, 0, obj->ibo_count);
	}

	glBindVertexArray(0);
	glUseProgram(0);
	glDisable(GL_CULL_FACE);
}

/**
* Water goes last because of blending
**/
void RenderOpenGL::water()
{
	if (! this->st->curr_map->water) return;

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glFrontFace(GL_CCW);

	glBindTexture(GL_TEXTURE_2D, this->st->curr_map->water->pixels);
	glUseProgram(this->shaders["water"]);
		
	if (this->waterobj->ibo_count == 0) this->createVBO(this->waterobj);
	
	glm::mat4 modelMatrix = glm::scale(
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, this->st->curr_map->water_level, 0.0f)),
		glm::vec3(this->st->curr_map->width, 1.0f, this->st->curr_map->height)
	);

	glm::mat4 MVP = this->projection * this->view * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(this->shaders["water"], "uMVP"), 1, GL_FALSE, glm::value_ptr(MVP));
		
	glBindVertexArray(this->waterobj->vao);
	glDrawArrays(GL_TRIANGLES, 0, this->waterobj->ibo_count);

	glBindVertexArray(0);
	glUseProgram(0);

	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}


/**
* Entities
**/
void RenderOpenGL::entities()
{
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	for (list<Entity*>::iterator it = st->entities.begin(); it != st->entities.end(); it++) {
		Entity *e = (*it);
		
		if (this->viewmode == 2 && e == this->render_player) continue;
		if (e->render == false) continue;

		AnimPlay *play = e->getAnimModel();
		if (play == NULL) continue;

		renderAnimPlay(play, e);
	}

	glDisable(GL_CULL_FACE);

	GLfloat em[] = {0.0, 0.0, 0.0, 0.0};
	glMaterialfv(GL_FRONT, GL_EMISSION, em);
}


/**
* Particle effects
**/
void RenderOpenGL::particles()
{
	unsigned int size = this->st->particles.size();
	if (size == 0) return;

	// Create some buffers, if required
	if (!this->particle_vao) {
		GLuint buffer;

		glGenVertexArrays(1,&(this->particle_vao));
		glBindVertexArray(this->particle_vao);

		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 24, BUFFER_OFFSET(0));		// Position
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 24, BUFFER_OFFSET(12));		// Colour
		
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

	} else {
		glBindVertexArray(this->particle_vao);
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * size, data, GL_DYNAMIC_DRAW);
	free(data);

	// Bind shader and draw
	glUseProgram(this->shaders["particles"]);
	
	glBindAttribLocation(this->shaders["particles"], 0, "vPosition");
	glBindAttribLocation(this->shaders["particles"], 1, "vColor");

	glm::mat4 MVP = this->projection * this->view;
	glUniformMatrix4fv(glGetUniformLocation(this->shaders["particles"], "uMVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	
	glDrawArrays(GL_POINTS, 0, size);
	
	glUseProgram(0);
	glBindVertexArray(0);
}


/**
* Heads-up display
**/
void RenderOpenGL::guichan()
{
	if (! st->hasDialogs()) return;

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_FOG);
	
	glLoadIdentity();
	this->st->gui->draw();
}


/**
* Heads-up display
**/
void RenderOpenGL::hud(HUD * hud)
{
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_FOG);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, this->virt_width, this->virt_height, 0.0, -1.0, 1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	hud->render(this);
}


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

