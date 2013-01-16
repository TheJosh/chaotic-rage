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


RenderOpenGLCompat::RenderOpenGLCompat(GameState * st) : Render3D(st)
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
}

RenderOpenGLCompat::~RenderOpenGLCompat()
{
}


/**
* Sets the screen size
**/
void RenderOpenGLCompat::setScreenSize(int width, int height, bool fullscreen)
{
	int flags;
	
	this->real_width = width;
	this->real_height = height;
	
	// SDL
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);

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
	
	// OpenGL env
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFlush();
	SDL_GL_SwapBuffers();
	
	mainViewport(1, 1);
}


void RenderOpenGLCompat::loadFont(string name, Mod * mod)
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
void RenderOpenGLCompat::initGuichan(gcn::Gui * gui, Mod * mod)
{
	gcn::OpenGLGraphics* graphics;
	gcn::ChaoticRageOpenGLSDLImageLoader* imageLoader;
	gcn::ImageFont* font;
	
	imageLoader = new gcn::ChaoticRageOpenGLSDLImageLoader(mod);
	gcn::Image::setImageLoader(imageLoader);
	
	try {
		// If you update the font, you gotta do in both cr and menu mods
		font = new gcn::ImageFont("fixedfont.bmp", " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789?!.,\"'");
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
void RenderOpenGLCompat::mainViewport(int s, int of)
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
	
	gluPerspective(45.0f, (float)this->virt_width / (float)this->virt_height, 1.0f, 150.f);
	//glScalef(-1.0f,1.0f,1.0f);
	//glTranslatef(0.f, 0.f, -120.0f);
	
	glMatrixMode(GL_MODELVIEW);
}


/**
* Enable debug drawing
**/
void RenderOpenGLCompat::enablePhysicsDebug()
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
void RenderOpenGLCompat::disablePhysicsDebug()
{
	this->st->physics->getWorld()->setDebugDrawer(NULL);
	delete this->physicsdebug;
	this->physicsdebug = NULL;
}


/**
* Saves a screenshot.
* Filename does NOT include extension
**/
void RenderOpenGLCompat::saveScreenshot(string filename)
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
SpritePtr RenderOpenGLCompat::int_loadSprite(SDL_RWops *rw, string filename)
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
void RenderOpenGLCompat::surfaceToOpenGL(SpritePtr sprite)
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load it
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sprite->orig->w, sprite->orig->h, 0, texture_format, GL_UNSIGNED_BYTE, sprite->orig->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
}


/**
* Loads a heightmap from the raw heightmap data (an array of floats).
* TODO: This should use TRIANGLE_STRIPS not TRIANGLES for rendering.
**/
void RenderOpenGLCompat::loadHeightmap()
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
void RenderOpenGLCompat::freeHeightmap()
{
	glDeleteBuffers(1, &this->ter_vboid);
}


/**
* Renders a sprite.
* Should only be used if the the caller was called by this classes 'Render' function.
**/
void RenderOpenGLCompat::renderSprite(SpritePtr sprite, int x, int y)
{
	renderSprite(sprite, x, y, sprite->w, sprite->h);
}

/**
* Renders a sprite.
* Should only be used if the the caller was called by this classes 'Render' function.
**/
void RenderOpenGLCompat::renderSprite(SpritePtr sprite, int x, int y, int w, int h)
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


void RenderOpenGLCompat::preGame()
{
	AnimModel *model = st->mm->getAnimModel("_test_cube");
	this->test = new AnimPlay(model);
	
	SDL_ShowCursor(SDL_DISABLE);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_MULTISAMPLE);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	glHint(GL_GENERATE_MIPMAP_HINT, GL_FASTEST);
	glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glHint(GL_FOG_HINT, GL_FASTEST);
}

void RenderOpenGLCompat::postGame()
{
	delete(this->test);
	
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
}


/**
* Clears all colour from a given pixel for a given sprite
**/
void RenderOpenGLCompat::clearPixel(int x, int y)
{
}

/**
* Free sprite memory
**/
void RenderOpenGLCompat::freeSprite(SpritePtr sprite)
{
	glDeleteTextures(1, &sprite->pixels);
	delete(sprite);
}

/**
* Returns sprite width
**/
int RenderOpenGLCompat::getSpriteWidth(SpritePtr sprite)
{
	return sprite->w;
}

/**
* Returns sprite height
**/
int RenderOpenGLCompat::getSpriteHeight(SpritePtr sprite)
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
void RenderOpenGLCompat::createVBO (WavefrontObj * obj)
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
	
	obj->ibo_count = obj->faces.size() * 3;
	obj->vbo = vboid;
	
	delete [] vertexes;
}


/**
* Call this before VBO render
**/
void RenderOpenGLCompat::preVBOrender()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glClientActiveTexture(GL_TEXTURE0);
}


/**
* Call this after VBO render, to clean up the OpenGL state
**/
void RenderOpenGLCompat::postVBOrender()
{
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


/**
* Renders an object.
* Uses VBOs, so you gotta call preVBOrender() beforehand, and postVBOrender afterwards()
**/
void RenderOpenGLCompat::renderObj (WavefrontObj * obj)
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
void RenderOpenGLCompat::renderAnimPlay(AnimPlay * play)
{
	AnimModel * model;
	
	model = play->getModel();
	if (model == NULL) return;
	
	int frame = play->getFrame();
	
	for (unsigned int d = 0; d < model->meshframes.size(); d++) {
		if (model->meshframes[d]->frame != frame) continue;
		if (model->meshframes[d]->mesh == NULL) continue;
		if (model->meshframes[d]->texture == NULL) continue;
		
		glBindTexture(GL_TEXTURE_2D, model->meshframes[d]->texture->pixels);
		
		glMaterialfv(GL_FRONT, GL_EMISSION, model->meshframes[d]->emission);
		
		glPushMatrix();
		
		glTranslatef(model->meshframes[d]->px, model->meshframes[d]->py, model->meshframes[d]->pz);
		glRotatef(model->meshframes[d]->rx, 1, 0, 0);
		glRotatef(model->meshframes[d]->ry, 0, 1, 0);
		glRotatef(model->meshframes[d]->rz, 0, 0, 1);
		
		
		// This is only temporary until the models get changed to be Y-up instead of Z-up
		glRotatef(270.0f, 1, 0, 0);
		
		
		glScalef(model->meshframes[d]->sx, model->meshframes[d]->sy, model->meshframes[d]->sz);
		
		this->renderObj(model->meshframes[d]->mesh);
		
		glPopMatrix();
	}
}


/**
* Draws text
*
* Note that the Y is for the baseline of the text.
**/
void RenderOpenGLCompat::renderText(string text, float x, float y, float r, float g, float b, float a)
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
void RenderOpenGLCompat::renderCharacter(char character)
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
unsigned int RenderOpenGLCompat::widthText(string text)
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
void RenderOpenGLCompat::render()
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
		hud(this->st->local_players[i]->hud);
	}

	mainViewport(0,1);
	guichan();

	SDL_GL_SwapBuffers();
}


/**
* Render debugging goodness for Bullet Physics
**/
void RenderOpenGLCompat::physics()
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
void RenderOpenGLCompat::background()
{
	if (this->render_player != NULL) {
		btTransform trans = this->render_player->getTransform();
		btVector3 euler;
		
		glLoadIdentity();
		glTranslatef(this->virt_width / 2, this->virt_height / 2, 0);
		glRotatef(RAD_TO_DEG(PhysicsBullet::QuaternionToYaw(trans.getRotation())), 0.f, 0.f, 1.f);
		glTranslatef(0.f - st->curr_map->background->w / 2.f, 0 - st->curr_map->background->h / 2.f, 0.f);
		this->renderSprite(st->curr_map->background, 0.f, 0.f);
	}
}


/**
* Main rotation for camera
**/
void RenderOpenGLCompat::mainRot()
{
	btTransform trans;
	float tilt, angle, dist;		// Up/down; left/right; distance of camera
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
		} else if (this->viewmode == 1) {
			tilt = 70.0f;
			dist = 50.0f;
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
	float cameray = dist * sin(DEG_TO_RAD(tilt)) + trans.getOrigin().y();
	float cameraz = dist * cos(DEG_TO_RAD(angle)) * cos(DEG_TO_RAD(tilt)) + trans.getOrigin().z();
	
	// OpenGL transforms
	glRotatef(tilt, 1.0f, 0.0f, 0.0f);
	glRotatef(360.0f - angle, 0.0f, 1.0f, 0.0f);
	glTranslatef(-camerax, -cameray, -cameraz);
}


/**
* Lighting
**/
void RenderOpenGLCompat::lights()
{
	unsigned int i;

	// Lights
	GLfloat dir_down[] = { 0.0f, 1.0f, 0.0f, 0.0f };
	GLfloat position[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat spot_down[] = { 0.0f, -1.0f, 0.0f };
	
	for (i = 0; i < st->curr_map->lights.size(); i++) {
		Light * l = st->curr_map->lights[i];
		
		if (l->type == 2 && this->render_player == NULL) continue;
		
		glPushMatrix();
			if (l->type == 1) {
				glLightfv(GL_LIGHT0 + i, GL_POSITION, dir_down);
				
			} else if (l->type == 2) {
				btTransform trans = this->render_player->getTransform();
				glTranslatef(trans.getOrigin().getX(), trans.getOrigin().getY() + 2.0f, trans.getOrigin().getZ());
				
				glLightfv(GL_LIGHT0 + i, GL_POSITION, position);
				
			} else if (l->type == 3) {
				glTranslatef(l->x, l->y, l->z);
				
				glLightfv(GL_LIGHT0 + i, GL_SPOT_DIRECTION, spot_down);
				glLightf(GL_LIGHT0 + i, GL_SPOT_CUTOFF, 40.0f);
				glLightf(GL_LIGHT0 + i, GL_SPOT_EXPONENT, 1.7f);
				glLightfv(GL_LIGHT0 + i, GL_POSITION, position);
			}
			
			glLightfv(GL_LIGHT0 + i, GL_AMBIENT, l->ambient);
			glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, l->diffuse);
			glLightfv(GL_LIGHT0 + i, GL_SPECULAR, l->specular);
			glEnable(GL_LIGHT0 + i);
		glPopMatrix();
		
		if (i == 7) break;
	}
}


/**
* Floor
**/
void RenderOpenGLCompat::map()
{
	glEnable(GL_NORMALIZE);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glBindTexture(GL_TEXTURE_2D, st->curr_map->terrain->pixels);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glPushMatrix();
	
	// TODO: Should this be a part of the mesh create instead of part of the render?
	glScalef(this->st->curr_map->heightmapScaleX(), 1.0f, this->st->curr_map->heightmapScaleY());
	
	// Main terrain
	glBindBuffer(GL_ARRAY_BUFFER, this->ter_vboid);
	glVertexPointer(3, GL_FLOAT, 32, BUFFER_OFFSET(0));
	glNormalPointer(GL_FLOAT, 32, BUFFER_OFFSET(12));
	glClientActiveTexture(GL_TEXTURE0);
	glTexCoordPointer(2, GL_FLOAT, 32, BUFFER_OFFSET(24));
	glDrawArrays(GL_TRIANGLES, 0, this->ter_size);
	
	glPopMatrix();
	
	// Static geometry meshes
	glFrontFace(GL_CCW);
	for (vector<MapMesh*>::iterator it = st->curr_map->meshes.begin(); it != st->curr_map->meshes.end(); it++) {
		glBindTexture(GL_TEXTURE_2D, (*it)->texture->pixels);

		glPushMatrix();

		btScalar m[16];
		(*it)->xform.getOpenGLMatrix(m);
		glMultMatrixf((GLfloat*)m);

		this->renderObj((*it)->mesh);

		glPopMatrix();
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glDisable(GL_CULL_FACE);


	

	// Water surface
	if (this->st->curr_map->water) {
 		glBindTexture(GL_TEXTURE_2D, st->curr_map->water->pixels);

		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, this->st->curr_map->height/10.0f);
			glVertex3f(0, this->st->curr_map->water_level, this->st->curr_map->height);
		
			glTexCoord2f(this->st->curr_map->width/10.0f, this->st->curr_map->height/10.0f);
			glVertex3f(this->st->curr_map->width, this->st->curr_map->water_level, this->st->curr_map->height);
		
			glTexCoord2f(this->st->curr_map->width/10.0f, 0.0f);
			glVertex3f(this->st->curr_map->width, this->st->curr_map->water_level, 0);

			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(0, this->st->curr_map->water_level, 0);
		glEnd();
	}
}


/**
* Entities
**/
void RenderOpenGLCompat::entities()
{
	preVBOrender();

	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	for (list<Entity*>::iterator it = st->entities.begin(); it != st->entities.end(); it++) {
		Entity *e = (*it);
		
		if (this->viewmode == 2 && e == this->render_player) continue;
		if (e->render == false) continue;

		AnimPlay *play = e->getAnimModel();
		if (play == NULL) continue;

		glPushMatrix();
		
		btTransform trans = e->getTransform();
		btScalar m[16];
		trans.getOpenGLMatrix(m);
		glMultMatrixf((GLfloat*)m);

		renderAnimPlay(play);
			
		glPopMatrix();
	}

	postVBOrender();

	glDisable(GL_CULL_FACE);

	GLfloat em[] = {0.0, 0.0, 0.0, 0.0};
	glMaterialfv(GL_FRONT, GL_EMISSION, em);
}


void RenderOpenGLCompat::particles()
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
void RenderOpenGLCompat::guichan()
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
void RenderOpenGLCompat::hud(HUD * hud)
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
