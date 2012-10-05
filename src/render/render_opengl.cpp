// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <algorithm>

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

#include <ft2build.h>
#include FT_FREETYPE_H

using namespace std;



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
	this->viewmode = 0;
	this->face = NULL;
	
	const SDL_VideoInfo* mode = SDL_GetVideoInfo();
	this->desktop_width = mode->current_w;
	this->desktop_height = mode->current_h;
	
	for (unsigned int i = 0; i < NUM_CHAR_TEX; i++) {
		this->char_tex[i].tex = 0;
	}
	
	q_tex = 4;
	q_alias = 4;
	q_general = 4;
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
		fprintf(stderr, "Unable to set %ix%i video: %s\n", width, height, SDL_GetError());
		exit(1);
	}
	
	char buff[100];
	sprintf(buff, "Chaotic Rage %s", VERSION);
	SDL_WM_SetCaption(buff, buff);
	

	// SDL_Image
	flags = IMG_INIT_PNG;
	int initted = IMG_Init(flags);
	if ((initted & flags) != flags) {
		fprintf(stderr, "Failed to init required png support!\n");
		exit(1);
	}
	
	
	// OpenGL
	if (atof((char*) glGetString(GL_VERSION)) < 2.0) {
		reportFatalError("OpenGL 2.0 or later is required, but not supported on this system.");
		exit(1);
	}
	
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "Glew Error: %s\n", glewGetErrorString(err));
		reportFatalError("Unable to init the library GLEW");
		exit(1);
	}
	
	// GL_ARB_framebuffer_object -> glGenerateMipmap
	if (! GL_ARB_framebuffer_object) {
		reportFatalError("OpenGL 3.0 or the extension 'GL_ARB_framebuffer_object' not available.");
		exit(1);
	}
	
	// Freetype
	int error;
	error = FT_Init_FreeType(&this->ft);
	if (error) {
		fprintf(stderr, "Freetype: Unable to init library\n");
		exit(1);
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
	
	font = new gcn::ImageFont("fixedfont.bmp", " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789?"); // .,!?-+/():;%&`'*#=[]\"
	gcn::Widget::setGlobalFont(font);
	
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
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluPerspective(45.0f, (float)this->virt_width / (float)this->virt_height, 1.0f, 1500.f);
	glScalef(-1.0f,1.0f,1.0f);
	glTranslatef(0.f, 0.f, -1250.0f);
	
	glMatrixMode(GL_MODELVIEW);
}


/**
* Enable debug drawing
**/
void RenderOpenGL::enablePhysicsDebug()
{
	this->physicsdebug = new GLDebugDrawer();
	this->physicsdebug->setDebugMode(
		btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawAabb | btIDebugDraw::DBG_DrawContactPoints | btIDebugDraw::DBG_NoDeactivation
	);
	this->st->physics->getWorld()->setDebugDrawer(this->physicsdebug);
}


/**
* Disable debug drawing
**/
void RenderOpenGL::disablePhysicsDebug()
{
	this->st->physics->getWorld()->setDebugDrawer(NULL);
	delete this->physicsdebug;
	this->physicsdebug = NULL;
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
	
	this->ter_size = st->curr_map->heightmap_w * st->curr_map->heightmap_h * 6;
	VBOvertex* vertexes = new VBOvertex[this->ter_size];
	
	j = 0;
	for( nZ = 0; nZ < st->curr_map->heightmap_h - 1; nZ += 1 ) {
		for( nX = 0; nX < st->curr_map->heightmap_w - 1; nX += 1 ) {
			
			// u = p2 - p1; v = p3 - p1
			btVector3 u = btVector3(nX + 1, nZ, st->curr_map->heightmapGet(nX + 1, nZ)) - btVector3(nX, nZ, st->curr_map->heightmapGet(nX, nZ));
			btVector3 v = btVector3(nX, nZ + 1, st->curr_map->heightmapGet(nX, nZ + 1)) - btVector3(nX, nZ, st->curr_map->heightmapGet(nX, nZ));
			
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
				vertexes[j].y = flZ;
				vertexes[j].z = st->curr_map->heightmapGet(flX, flZ);
				vertexes[j].nx = normal.x();
				vertexes[j].ny = normal.y();
				vertexes[j].nz = normal.z();
				vertexes[j].tx = flX / st->curr_map->heightmap_w;
				vertexes[j].ty = flZ / st->curr_map->heightmap_h;
				j++;
			}
		}
	}
	
	glGenBuffers(1, &this->ter_vboid);
	glBindBuffer(GL_ARRAY_BUFFER, this->ter_vboid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VBOvertex) * this->ter_size, vertexes, GL_STATIC_DRAW);
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
	AnimModel *model = st->mm->getAnimModel("_test_cube");
	this->test = new AnimPlay(model);
	
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
}

void RenderOpenGL::postGame()
{
	delete(this->test);
	
	SDL_ShowCursor(SDL_ENABLE);
	mainViewport(1, 1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_MULTISAMPLE);

	GLfloat em[] = {0.0, 0.0, 0.0, 0.0};
	glMaterialfv(GL_FRONT, GL_EMISSION, em);
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
* For VBO pointer offsets
**/
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

/**
* Builds a VBO for this object
**/
void RenderOpenGL::createVBO (WavefrontObj * obj)
{
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
		
		vertexes[j].x = v->x * -1.0f; vertexes[j].y = v->y; vertexes[j].z = v->z;
		vertexes[j].nx = vn->x; vertexes[j].ny = vn->y; vertexes[j].nz = vn->z;
		vertexes[j].tx = t->x; vertexes[j].ty = t->y * -1.0f;
		j++;
		
		
		v = &obj->vertexes.at(f->v2 - 1);
		vn = &obj->normals.at(f->n2 - 1);
		t = &obj->texuvs.at(f->t2 - 1);
		
		vertexes[j].x = v->x * -1.0f; vertexes[j].y = v->y; vertexes[j].z = v->z;
		vertexes[j].nx = vn->x; vertexes[j].ny = vn->y; vertexes[j].nz = vn->z;
		vertexes[j].tx = t->x; vertexes[j].ty = t->y * -1.0f;
		j++;
		
		
		v = &obj->vertexes.at(f->v3 - 1);
		vn = &obj->normals.at(f->n3 - 1);
		t = &obj->texuvs.at(f->t3 - 1);
		
		vertexes[j].x = v->x * -1.0f; vertexes[j].y = v->y; vertexes[j].z = v->z;
		vertexes[j].nx = vn->x; vertexes[j].ny = vn->y; vertexes[j].nz = vn->z;
		vertexes[j].tx = t->x; vertexes[j].ty = t->y * -1.0f;
		j++;
		
	}
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(VBOvertex) * obj->faces.size() * 3, vertexes, GL_STATIC_DRAW);
	
	obj->ibo_count = obj->faces.size() * 3;
	obj->vbo = vboid;
	
	delete [] vertexes;
}


/**
* Call this before VBO render
**/
void RenderOpenGL::preVBOrender()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glClientActiveTexture(GL_TEXTURE0);
}


/**
* Call this after VBO render, to clean up the OpenGL state
**/
void RenderOpenGL::postVBOrender()
{
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


/**
* Renders an object.
* Uses VBOs, so you gotta call preVBOrender() beforehand, and postVBOrender afterwards()
**/
void RenderOpenGL::renderObj (WavefrontObj * obj)
{
	if (obj->ibo_count == 0) this->createVBO(obj);
	
	glBindBuffer(GL_ARRAY_BUFFER, obj->vbo);

	glVertexPointer(3, GL_FLOAT, 32, BUFFER_OFFSET(0));
	glNormalPointer(GL_FLOAT, 32, BUFFER_OFFSET(12));
	glTexCoordPointer(2, GL_FLOAT, 32, BUFFER_OFFSET(24));

	glDrawArrays(GL_TRIANGLES, 0, obj->ibo_count);
}


/**
* Renders an animation.
* Uses VBOs, so you gotta call preVBOrender() beforehand, and postVBOrender afterwards()
**/
void RenderOpenGL::renderAnimPlay(AnimPlay * play)
{
	AnimModel * model;
	
	model = play->getModel();
	if (model == NULL) return;
	
	int frame = play->getFrame();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	for (unsigned int d = 0; d < model->meshframes.size(); d++) {
		if (model->meshframes[d]->frame != frame) continue;
		if (model->meshframes[d]->mesh == NULL) continue;
		if (model->meshframes[d]->texture == NULL) continue;
		
		glBindTexture(GL_TEXTURE_2D, model->meshframes[d]->texture->pixels);
		
		glMaterialfv(GL_FRONT, GL_EMISSION, model->meshframes[d]->emission);
		
		glPushMatrix();
		
		// TODO: Fix for new physics
		//if (model->meshframes[d]->do_angle) {
		//	glRotatef(0 - angle, 0, 0, 1);
		//}
		
		glTranslatef(model->meshframes[d]->px, model->meshframes[d]->py, model->meshframes[d]->pz);
		glRotatef(model->meshframes[d]->rx, 1, 0, 0);
		glRotatef(model->meshframes[d]->ry, 0, 1, 0);
		glRotatef(model->meshframes[d]->rz, 0, 0, 1);
		glScalef(model->meshframes[d]->sx, model->meshframes[d]->sy, model->meshframes[d]->sz);
		
		this->renderObj(model->meshframes[d]->mesh);
		
		glPopMatrix();
	}

	glDisable(GL_BLEND);
}


/**
* Draws text
*
* Note that the Y is for the baseline of the text.
**/
void RenderOpenGL::renderText(string text, float x, float y, float r, float g, float b)
{
	if (face == NULL) return;

	glPushMatrix();
	glTranslatef(x, y, 0);
	
	unsigned int n;
	
	glColor3f(r, g, b);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	for ( n = 0; n < text.length(); n++ ) {
		this->renderCharacter(text[n]);
	}
	
	glDisable(GL_BLEND);
	glColor3f(1, 1, 1);
	
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
* This is the main render function.
* It calls all of the sub-render parts which are defined below.
**/
void RenderOpenGL::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (unsigned int i = 0; i < this->st->num_local; i++) {
		this->render_player = this->st->local_players[i]->p;
		mainViewport(i, this->st->num_local);
		
		//background();
		mainRot();
		lights();
		map();
		entities();
		particles();
		if (physicsdebug != NULL) physics();
		guichan();
		hud(this->st->local_players[i]->hud);
	}
	
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
* Background
**/
void RenderOpenGL::background()
{
	if (this->render_player != NULL) {
		btTransform trans;
		this->render_player->body->getMotionState()->getWorldTransform(trans);
		btVector3 euler;
		PhysicsBullet::QuaternionToEulerXYZ(trans.getRotation(), euler);
		
		glLoadIdentity();
		glTranslatef(this->virt_width / 2, this->virt_height / 2, 0);
		glRotatef(RAD_TO_DEG(euler.z()), 0.f, 0.f, 1.f);
		glTranslatef(0.f - st->curr_map->background->w / 2.f, 0 - st->curr_map->background->h / 2.f, 0.f);
		this->renderSprite(st->curr_map->background, 0.f, 0.f);
	}
}


/**
* Camera spin when player is dead
**/
void RenderOpenGL::deadRot()
{
	static float angle = 22.0f;
	
	float hw = st->curr_map->width / 2.f;
	float hh = st->curr_map->width / 2.f;
	
	// Proper angle
	glRotatef(90.0f, 0, 0, 1);
	glTranslatef(0.f - hw, 0.f - hh, 1000.f);
	glRotatef(50.0f, 0, 1, 0);
	
	// Map spin
	glTranslatef(hw, hh, 0.f);
	glRotatef(angle, 0, 0, 1);
	glTranslatef(0.f - hw, 0.f - hh, 0.f);
	
	angle += 0.05f;
}


/**
* Main rotation for camera
**/
void RenderOpenGL::mainRot()
{
	btTransform trans;

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	if (this->render_player == NULL) {
		this->deadRot();
		return;
	}
	
	if (this->viewmode == 1) {				// Top
		glRotatef(180, 0, 0, 1);
		glTranslatef(0,87,731);
		glRotatef(10, 1, 0, 0);
		glTranslatef(0,0,-10);
		
	} else if (this->viewmode == 0) {		// Behind (3rd person)
		glRotatef(180, 0, 0, 1);
		glTranslatef(0,483,1095);
		glRotatef(74, 1, 0, 0);
	
	} else if (this->viewmode == 2) {		// First person
		glTranslatef(0,1220,-380);
		glRotatef(80, 1, 0, 0);
		
	}
	
	if (this->render_player->drive) {
		this->render_player->drive->body->getMotionState()->getWorldTransform(trans);
		btVector3 euler;
		PhysicsBullet::QuaternionToEulerXYZ(trans.getRotation(), euler);
		glRotatef(RAD_TO_DEG(euler.z()), 0.f, 0.f, 1.f);

	} else {
		this->render_player->body->getMotionState()->getWorldTransform(trans);
		btVector3 euler;
		PhysicsBullet::QuaternionToEulerXYZ(trans.getRotation(), euler);
		glRotatef(RAD_TO_DEG(euler.z()), 0.f, 0.f, 1.f);
	}
	
	glTranslatef(0.f - trans.getOrigin().getX(), 0.f - trans.getOrigin().getY(), 500.f - trans.getOrigin().getZ());
	
	if (this->viewmode == 0) {
		glEnable(GL_FOG);
		glFogi(GL_FOG_MODE, GL_LINEAR);
		glFogf(GL_FOG_START, 400);
		glFogf(GL_FOG_END, 200);
		glFogf(GL_FOG_DENSITY, 0.3f);
	}
}


/**
* Lighting
**/
void RenderOpenGL::lights()
{
	unsigned int i;

	// Lights
	GLfloat position[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat spot_torch[] = { -1.0f, -1.0f, 0.0f };
	
	for (i = 0; i < st->curr_map->lights.size(); i++) {
		Light * l = st->curr_map->lights[i];
		
		if (l->type == 2 && this->render_player == NULL) continue;
		
		glPushMatrix();
			if (l->type == 1) {
				glTranslatef(l->x, l->y, l->z);
				
			} else if (l->type == 2) {
				btTransform trans;
				this->render_player->getRigidBody()->getMotionState()->getWorldTransform(trans);
				glTranslatef(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
				
				btVector3 euler;
				PhysicsBullet::QuaternionToEulerXYZ(trans.getRotation(), euler);
				glRotatef(RAD_TO_DEG(-euler.z()) + 45.0f, 0.f, 0.f, 1.f);
				glRotatef(15.0f, 1, 0, 0);
				
				glLightfv(GL_LIGHT0 + i, GL_SPOT_DIRECTION, spot_torch);
				glLightf(GL_LIGHT0 + i, GL_SPOT_CUTOFF, 20);
				glLightf(GL_LIGHT0 + i, GL_SPOT_EXPONENT, 2);
			}
			
			glLightfv(GL_LIGHT0 + i, GL_AMBIENT, l->ambient);
			glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, l->diffuse);
			glLightfv(GL_LIGHT0 + i, GL_SPECULAR, l->specular);
			glLightfv(GL_LIGHT0 + i, GL_POSITION, position);
			glEnable(GL_LIGHT0 + i);
		glPopMatrix();
		
		if (i == 7) break;
	}
}


/**
* Floor
**/
void RenderOpenGL::map()
{
	glEnable(GL_NORMALIZE);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glFrontFace(GL_CW);
	glBindTexture(GL_TEXTURE_2D, st->curr_map->terrain->pixels);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glPushMatrix();
	
	// TODO: Should this be a part of the mesh create instead of part of the render?
	glScalef(this->st->curr_map->heightmapScaleX(), this->st->curr_map->heightmapScaleY(), 1.0);
	
	glBindBuffer(GL_ARRAY_BUFFER, this->ter_vboid);
	glVertexPointer(3, GL_FLOAT, 32, BUFFER_OFFSET(0));
	glNormalPointer(GL_FLOAT, 32, BUFFER_OFFSET(12));
	glClientActiveTexture(GL_TEXTURE0);
	glTexCoordPointer(2, GL_FLOAT, 32, BUFFER_OFFSET(24));
	glDrawArrays(GL_TRIANGLES, 0, this->ter_size);
	
	glPopMatrix();
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glDisable(GL_CULL_FACE);


	// Water surface
	if (this->st->curr_map->water) {
 		glBindTexture(GL_TEXTURE_2D, st->curr_map->water->pixels);
		glEnable(GL_BLEND);

		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, this->st->curr_map->height/10.0f);
			glVertex3f(0, this->st->curr_map->height, this->st->curr_map->water_level);
		
			glTexCoord2f(this->st->curr_map->width/10.0f, this->st->curr_map->height/10.0f);
			glVertex3f(this->st->curr_map->width, this->st->curr_map->height, this->st->curr_map->water_level);
		
			glTexCoord2f(this->st->curr_map->width/10.0f, 0.0f);
			glVertex3f(this->st->curr_map->width, 0, this->st->curr_map->water_level);

			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(0, 0, this->st->curr_map->water_level);
		glEnd();
	}
}


/**
* Entities
**/
void RenderOpenGL::entities()
{
	preVBOrender();

	glFrontFace(GL_CCW);		// almost everything is wound wrong...
	glEnable(GL_CULL_FACE);

	for (list<Entity*>::iterator it = st->entities.begin(); it != st->entities.end(); it++) {
		Entity *e = (*it);
		
		if (this->viewmode == 2 && e == this->render_player) continue;
		if (e->render == false) continue;

		AnimPlay *play = e->getAnimModel();
		if (play == NULL) continue;

		glPushMatrix();
			
		btTransform trans;
		e->getRigidBody()->getMotionState()->getWorldTransform(trans);
		btScalar m[16];
		trans.getOpenGLMatrix(m);
		glMultMatrixf((GLfloat*)m);

		if (e == this->render_player) {
			this->render_player->body->getMotionState()->getWorldTransform(trans);
			btVector3 euler;
			PhysicsBullet::QuaternionToEulerXYZ(trans.getRotation(), euler);
			glRotatef(RAD_TO_DEG(-euler.z()) * 2.0f, 0.f, 0.f, 1.f);
		}

		renderAnimPlay(play);
			
		glPopMatrix();
	}

	postVBOrender();

	glDisable(GL_CULL_FACE);

	GLfloat em[] = {0.0, 0.0, 0.0, 0.0};
	glMaterialfv(GL_FRONT, GL_EMISSION, em);
}


void RenderOpenGL::particles()
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glPointSize(10.f);
	glBegin(GL_POINTS);
	for (list<NewParticle*>::iterator it = this->st->particles.begin(); it != this->st->particles.end(); it++) {
		glColor4f((*it)->r, (*it)->g, (*it)->b, 1.0f);
		glVertex3f((*it)->pos.x(), (*it)->pos.y(), (*it)->pos.z());
	}
	glEnd();
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
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

