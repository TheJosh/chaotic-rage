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
#include "rage.h"

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


//static bool ZIndexPredicate(const Entity * e1, const Entity * e2)
//{
//	return e1->z < e2->z;
//}


RenderOpenGL::RenderOpenGL(GameState * st) : Render(st)
{
	this->screen = NULL;
	this->viewmode = 0;

	const SDL_VideoInfo* mode = SDL_GetVideoInfo();
	this->desktop_width = mode->current_w;
	this->desktop_height = mode->current_h;

	for (unsigned int i = 0; i < NUM_CHAR_TEX; i++) {
		this->char_tex[i].tex = 0;
	}
	
	
	// top
	tx = 0;
	ty = 193;
	tz = 620;
	rx = 10;
	
	// 3rd
	/*tx = 0;
	ty = 1039;
	tz = 81;
	rx = 60;*/
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
	
	flags = SDL_OPENGL;
	if (fullscreen) flags |= SDL_FULLSCREEN;
	
	this->screen = SDL_SetVideoMode(width, height, 32, flags);
	if (screen == NULL) {
		fprintf(stderr, "Unable to set %ix%i video: %s\n", width, height, SDL_GetError());
		exit(1);
	}
	
	SDL_WM_SetCaption("Chaotic Rage", "Chaotic Rage");
	

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
		exit(1);
	}
	
	
	// Freetype
	int error;
	error = FT_Init_FreeType(&this->ft);
	if (error) {
		fprintf(stderr, "Freetype: Unable to init library\n");
		exit(1);
	}
	
	error = FT_New_Face(this->ft, "orbitron-black.otf", 0, &this->face);
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
	
	
	// Loaded textures
	if (loaded.size() > 0) {
		for (unsigned int i = 0; i != loaded.size(); i++) {
			this->surfaceToOpenGL(loaded.at(i));
		}
	}
	
	// Loaded FreeType character textures
	for (unsigned int i = 0; i < NUM_CHAR_TEX; i++) {
		if (this->char_tex[i].tex) {
			glDeleteTextures(1, &this->char_tex[i].tex);
			this->char_tex[i].tex = 0;
		}
	}
	
	
	// OpenGL env
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	
	mainViewport(1, 1);
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
	glScalef (1.0f, -1.0f, 1.0f);
	glTranslatef(0 - (this->virt_width / 2), 0 - (this->virt_height / 2), -1250.0f);

	glMatrixMode(GL_MODELVIEW);
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
		fprintf(stderr, "Couldn't load sprite '%s'.\n", filename.c_str());
		load_err = true;
		return NULL;
	}
	
	// Checks
	if ((surf->w & (surf->w - 1)) != 0) {
		DEBUG("Bitmap '%s' width is not a power of 2.\n", filename.c_str());
	}
	
	if ((surf->h & (surf->h - 1)) != 0) {
		DEBUG("Bitmap '%s' height is not a power of 2.\n", filename.c_str());
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
	glGenTextures(1, &sprite->pixels);
	glBindTexture(GL_TEXTURE_2D, sprite->pixels);
	
	// Set stretching properties
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	// Load it
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sprite->orig->w, sprite->orig->h, 0, texture_format, GL_UNSIGNED_BYTE, sprite->orig->pixels);
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
	AnimModel *model = st->getDefaultMod()->getAnimModel("_test_cube");
	this->test = new AnimPlay(model);
	
	SDL_ShowCursor(SDL_DISABLE);
}

void RenderOpenGL::postGame()
{
	delete(this->test);
	
	SDL_ShowCursor(SDL_ENABLE);
	mainViewport(1, 1);
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
		
		vertexes[j].x = v->x * -1.0; vertexes[j].y = v->y; vertexes[j].z = v->z;
		vertexes[j].nx = vn->x; vertexes[j].ny = vn->y; vertexes[j].nz = vn->z;
		vertexes[j].tx = t->x; vertexes[j].ty = t->y * -1.0;
		j++;
		
		
		v = &obj->vertexes.at(f->v2 - 1);
		vn = &obj->normals.at(f->n2 - 1);
		t = &obj->texuvs.at(f->t2 - 1);
		
		vertexes[j].x = v->x * -1.0; vertexes[j].y = v->y; vertexes[j].z = v->z;
		vertexes[j].nx = vn->x; vertexes[j].ny = vn->y; vertexes[j].nz = vn->z;
		vertexes[j].tx = t->x; vertexes[j].ty = t->y * -1.0;
		j++;
		
		
		v = &obj->vertexes.at(f->v3 - 1);
		vn = &obj->normals.at(f->n3 - 1);
		t = &obj->texuvs.at(f->t3 - 1);
		
		vertexes[j].x = v->x * -1.0; vertexes[j].y = v->y; vertexes[j].z = v->z;
		vertexes[j].nx = vn->x; vertexes[j].ny = vn->y; vertexes[j].nz = vn->z;
		vertexes[j].tx = t->x; vertexes[j].ty = t->y * -1.0;
		j++;
		
	}
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(VBOvertex) * obj->faces.size() * 3, vertexes, GL_STATIC_DRAW);
	
	obj->ibo_count = obj->faces.size() * 3;
	obj->vbo = vboid;
	
	delete [] vertexes;
}


/**
* Renders an object
**/
void RenderOpenGL::renderObj (WavefrontObj * obj)
{
	if (obj->ibo_count == 0) this->createVBO(obj);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glBindBuffer(GL_ARRAY_BUFFER, obj->vbo);
	glVertexPointer(3, GL_FLOAT, 32, BUFFER_OFFSET(0));
	glNormalPointer(GL_FLOAT, 32, BUFFER_OFFSET(12));
	glClientActiveTexture(GL_TEXTURE0);
	glTexCoordPointer(2, GL_FLOAT, 32, BUFFER_OFFSET(24));
	
	glDrawArrays(GL_TRIANGLES, 0, obj->ibo_count);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


void RenderOpenGL::renderAnimPlay(AnimPlay * play, int angle)
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
		
		if (model->meshframes[d]->do_angle) {
			glRotatef(0 - angle, 0, 0, 1);
		}
		
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
void RenderOpenGL::renderText(string text, int x, int y)
{
	glPushMatrix();
	glTranslatef(x, y, 0);
	
	unsigned int n;
	
	glColor3f(1,1,1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	for ( n = 0; n < text.length(); n++ ) {
		this->renderCharacter(text[n]);
	}
	
	glDisable(GL_BLEND);
	glColor3f(1,1,1);
	
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
	glTranslatef(c->x, 0 - c->y, 0);
	glBindTexture(GL_TEXTURE_2D, c->tex);
	
	glBegin(GL_QUADS);
		glTexCoord2d(0,c->ty); glVertex2f(0,c->h);
		glTexCoord2d(0,0); glVertex2f(0,0);
		glTexCoord2d(c->tx,0); glVertex2f(c->w,0);
		glTexCoord2d(c->tx,c->ty); glVertex2f(c->w,c->h);
	glEnd();
	
	glPopMatrix();
	glTranslatef(slot->advance.x >> 6, 0, 0);
}





/**
* This is the main render function.
* It calls all of the sub-render parts which are defined below.
**/
void RenderOpenGL::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (unsigned int i = 0; i < this->st->num_local; i++) {
		this->render_player = this->st->local_players[i];
		mainViewport(i, this->st->num_local);
		
		background();
		mainRot();
		lights();
		
		glDisable(GL_LIGHTING);
		
		map();
		
		glPushMatrix();
		glTranslatef(100, 100, 0);
		renderAnimPlay(this->test, 0);
		glPopMatrix();
		
		
		entities();
		hud();
	}
	
	
	char buf[50];
	sprintf(buf, "%i %i %i %i", tx, ty, tz, rx);
	this->renderText(buf, 200, 200);
	
	
	SDL_GL_SwapBuffers();
}


/**
* Background
**/
void RenderOpenGL::background()
{
	if (this->render_player != NULL) {
		glLoadIdentity();
		glTranslatef(this->virt_width / 2, this->virt_height / 2, 0);
		glRotatef(this->render_player->angle, 0, 0, 1);
		glTranslatef(0 - st->curr_map->background->w / 2, 0 - st->curr_map->background->h / 2, 0);
		this->renderSprite(st->curr_map->background, 0, 0);
	}
}


/**
* Main rotation for camera
**/
void RenderOpenGL::mainRot()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// Main map rotation
	glLoadIdentity();
	glTranslatef(this->virt_width / 2, this->virt_height / 2, 0);
	
	
	if (this->render_player == NULL) {
		glDisable(GL_LIGHTING);
		glRotatef(22, 0, 0, 1);
		glRotatef(12, 1, 0, 0);
		glTranslatef(0 - st->curr_map->width / 2, 0 - st->curr_map->height / 2, 0);
		
	} else {
		//glTranslatef(tx,ty,tz);
		//glRotatef(rx, 1, 0, 0);
		
		if (this->viewmode == 0) {			// Top
			glTranslatef(0,87,731);
			glRotatef(10, 1, 0, 0);
			
		} else if (this->viewmode == 1) {		// Behind (3rd person)
			glTranslatef(0,483,1095);
			glRotatef(74, 1, 0, 0);
			
		} else if (this->viewmode == 2) {		// First person
			glTranslatef(0,1220,-380);
			glRotatef(80, 1, 0, 0);
			
		}
		
		glRotatef(this->render_player->angle, 0, 0, 1);
		
		btTransform trans;
		this->render_player->getRigidBody()->getMotionState()->getWorldTransform(trans);
		glTranslatef(0 - trans.getOrigin().getX(), 0 - trans.getOrigin().getY(), 500 - trans.getOrigin().getZ());
	}
	
	if (this->viewmode == 0) {
		//glTranslatef(0, 0, getRandom(0, this->st->getEntropy(1)));
		
	} else {
		glEnable(GL_FOG);
		glFogi (GL_FOG_MODE, GL_LINEAR);
		glFogf (GL_FOG_START, 400);
		glFogf (GL_FOG_END, 200);
		glFogf (GL_FOG_DENSITY, 0.3);
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
				
				glRotatef(0 - this->render_player->angle + 40, 0, 0, 1);
				glRotatef(15, 1, 0, 0);
				
				glLightfv(GL_LIGHT0 + i, GL_SPOT_DIRECTION, spot_torch);
				glLightf(GL_LIGHT0 + i, GL_SPOT_CUTOFF, 20);
				glLightf(GL_LIGHT0 + i, GL_SPOT_EXPONENT, 2);
				
				
				/*glPushMatrix();
				glScalef(10,10,10);
				renderAnimPlay(this->test, 0);
				glPopMatrix();*/
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
	unsigned int i;
	int x, y;

	GLfloat emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_EMISSION, emission);
	
	// Render map
	for (i = 0; i < st->curr_map->areas.size(); i++) {
		Area * a = st->curr_map->areas[i];
		
		glPushMatrix();
		
		x = a->x + a->width / 2;
		y = a->y + a->height / 2;
		
		glTranslatef(x, y, 0);
		glRotatef(0 - a->angle, 0, 0, 1);
		glTranslatef(0 - x, 0 - y, 0);
		
		glBindTexture(GL_TEXTURE_2D, a->type->texture->pixels);
		
		a->width = a->height = 1;
		
		float texw = 1.0;
		float texh = 1.0;
		if (! a->type->stretch) {
			texw = ((float)a->width) / ((float)a->type->texture->w);
			texh = ((float)a->height) / ((float)a->type->texture->h);
		}
		
		glEnable(GL_NORMALIZE);
		glLightModeli (GL_LIGHT_MODEL_COLOR_CONTROL,GL_SEPARATE_SPECULAR_COLOR);
		glShadeModel(GL_SMOOTH);
		
		glNormal3f(0, 0, 1);
		
		glBegin(GL_QUADS);
			for (int x = 0; x < st->curr_map->width; x += a->width) {
			for (int y = 0; y < st->curr_map->height; y += a->width) {
				
				// Bottom-left vertex (corner)
				glTexCoord2f( 0.0, texh );
				glVertex3i( x, y + a->height, 0 );
				
				// Bottom-right vertex (corner)
				glTexCoord2f( texw, texh );
				glVertex3i( x + a->width, y + a->height, 0 );
				
				// Top-right vertex (corner)
				glTexCoord2f( texw, 0.0 );
				glVertex3i( x + a->width, y, 0 );
				
				// Top-left vertex (corner)
				glTexCoord2f( 0.0, 0.0 );
				glVertex3i( x, y, 0 );
				
			}
			}
		glEnd();
		
		glPopMatrix();
	}
}


/**
* Entities
**/
void RenderOpenGL::entities()
{
	for (list<Entity*>::iterator it = st->entities.begin(); it != st->entities.end(); it++) {
		Entity *e = (*it);
		
		if (this->viewmode == 2 && e == this->render_player) continue;
		
		AnimPlay *play = e->getAnimModel();
		
		if (play != NULL) {
			glPushMatrix();
			
			btTransform trans;
			e->getRigidBody()->getMotionState()->getWorldTransform(trans);
			glTranslatef(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
			
			renderAnimPlay(play, e->angle);
			renderAnimPlay(this->test, 0);
			
			glPopMatrix();
		}
	}
}


/**
* Testing: show collide boxes
**/
void RenderOpenGL::collides()
{
	for (list<CollideBox*>::iterator it = st->collideboxes.begin(); it != st->collideboxes.end(); it++) {
		CollideBox *c = (*it);
		
		glPushMatrix();
		
		glTranslatef(c->x, c->y, 100);
		glScalef(c->radius, c->radius, c->radius);
		renderAnimPlay(this->test, 0);
		
		glPopMatrix();
	}
}


/**
* Heads-up display
**/
void RenderOpenGL::hud()
{
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_FOG);
	
	glLoadIdentity();
	glTranslatef(0, 0, 40);
	st->hud->render(this);
}


