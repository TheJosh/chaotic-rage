// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <algorithm>
#include <GL/gl.h>
#if defined(__WIN32__)
	#include <GL/glext.h>
#endif
#include <GL/glu.h>
#include <SDL_image.h>
#include <math.h>
#include "rage.h"

using namespace std;



//static bool ZIndexPredicate(const Entity * e1, const Entity * e2)
//{
//	return e1->z < e2->z;
//}


RenderOpenGL::RenderOpenGL(GameState * st) : Render(st)
{
	this->screen = NULL;
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
	
	// TODO: we need to keep track of all textures
	// because when this method runs again, we need to re-load everything
	// back into OpenGL
	// Priority: Medium
	
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	
	flags = SDL_OPENGL;
	if (fullscreen) flags |= SDL_FULLSCREEN;
	
	this->screen = SDL_SetVideoMode(width, height, 32, flags);
	if (screen == NULL) {
		fprintf(stderr, "Unable to set %ix%i video: %s\n", width, height, SDL_GetError());
		exit(1);
	}
	
	if (atof((char*) glGetString(GL_VERSION)) < 2.0) {
		fprintf(stderr, "OpenGL 2.0 or later is required, but not supported on this system.");
		exit(1);
	}
	
	// The 'virtual' size is 1000px high, with the proper width for your monitor.
	this->virt_height = 1000;
	this->virt_width = (int) floor(this->virt_height * (float)width / (float)height);
	
	SDL_WM_SetCaption("Chaotic Rage", "Chaotic Rage");
	SDL_ShowCursor(SDL_DISABLE);
	
	flags = IMG_INIT_PNG;
	int initted = IMG_Init(flags);
	if ((initted & flags) != flags) {
		fprintf(stderr, "Failed to init required png support!\n");
		exit(1);
	}
	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	glViewport(0, 0, width, height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluPerspective(45.0f, 1, 1.f, 1500.f);
	glScalef (1.0f, -1.f, 1.0f);
	glTranslatef(-500, -500, -1250.0f);
	
	//glOrtho(0.0f, this->virt_width, this->virt_height, 0.0f, -1.0f, 1.0f);
	
	//glFrustum(0.0f, this->virt_width, this->virt_height, 0.0f, 1, 500);
	//glTranslatef(0, 0, -490.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


/**
* Internal sprite loading from a SDL_RWops
**/
SpritePtr RenderOpenGL::int_loadSprite(SDL_RWops *rw, string filename)
{
	GLenum texture_format;
	GLint num_colors;
	SDL_Surface * surf;
	
	surf = IMG_Load_RW(rw, 0);
	if (surf == NULL) {
		fprintf(stderr, "Couldn't load sprite '%s'.\n", filename.c_str());
		load_err = true;
		return NULL;
	}
	
	if ((surf->w & (surf->w - 1)) != 0) {
		DEBUG("Bitmap '%s' width is not a power of 2.\n", filename.c_str());
	}
	
	if ((surf->h & (surf->h - 1)) != 0) {
		DEBUG("Bitmap '%s' height is not a power of 2.\n", filename.c_str());
	}
	
	// Determine OpenGL import type
	num_colors = surf->format->BytesPerPixel;
	if (num_colors == 4) {
		if (surf->format->Rmask == 0x000000ff) {
			texture_format = GL_RGBA;
		} else {
			texture_format = GL_BGRA;
		}
		
	} else if (num_colors == 3) {
		if (surf->format->Rmask == 0x000000ff) {
			texture_format = GL_RGB;
		} else {
			texture_format = GL_BGR;
		}
		
	} else {
		fprintf(stderr, "Bitmap '%s' not in 32-bit or 24-bit colour; unable to load into OpenGL\n", filename.c_str());
		load_err = true;
		return NULL;
	}
	
	// Create the sprite object
	SpritePtr sprite = new struct sprite();
	sprite->w = surf->w;
	sprite->h = surf->h;
	sprite->orig = surf;
	
	// Open texture handle
	glGenTextures(1, &sprite->pixels);
	glBindTexture(GL_TEXTURE_2D, sprite->pixels);
	
	// Set stretching properties
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surf->w, surf->h, 0, texture_format, GL_UNSIGNED_BYTE, surf->pixels);
	
	//SDL_FreeSurface(surf);
	
	return sprite;
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
		glVertex2i( x, y + w );
		
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
}

void RenderOpenGL::postGame()
{
}

/**
* Clears all colour from a given pixel for a given sprite
**/
void RenderOpenGL::clearPixel(int x, int y)
{
	// TODO: code this
	// Priority: Low
	//setPixel(walls->surf, x, y, this->colourkey);
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
* Renders an object
*
* TODO: Something should be done some time to switch this to VBOs
**/
static void renderObj (WavefrontObj * obj)
{
	glBegin(GL_TRIANGLES);
	for (unsigned int i = 0; i < obj->faces.size(); i++) {
		Face * f = &obj->faces.at(i);
		Vertex * v;
		TexUV * t;
		
		// Vertex 1
		if (f->n1 != 0) {
			v = &obj->normals.at(f->n1 - 1);
			glNormal3f(v->x, v->y, v->z);
		}
		
		if (f->t1 != 0) {
			t = &obj->texuvs.at(f->t1 - 1);
			glTexCoord2f(t->x, 1.0 - t->y);
		}
		
		v = &obj->vertexes.at(f->v1 - 1);
		glVertex3f(v->x, v->y, v->z);
		
		
		// Vertex 2
		if (f->n2 != 0) {
			v = &obj->normals.at(f->n2 - 1);
			glNormal3f(v->x, v->y, v->z);
		}
		
		if (f->t2 != 0) {
			t = &obj->texuvs.at(f->t2 - 1);
			glTexCoord2f(t->x, 1.0 - t->y);
		}
		
		v = &obj->vertexes.at(f->v2 - 1);
		glVertex3f(v->x, v->y, v->z);
		
		
		// Vertex 3
		if (f->n3 != 0) {
			v = &obj->normals.at(f->n3 - 1);
			glNormal3f(v->x, v->y, v->z);
		}
		
		if (f->t3 != 0) {
			t = &obj->texuvs.at(f->t3 - 1);
			glTexCoord2f(t->x, 1.0 - t->y);
		}
		
		v = &obj->vertexes.at(f->v3 - 1);
		glVertex3f(v->x, v->y, v->z);
	}
	glEnd();
}


static void renderAnimPlay(AnimPlay * play)
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
		
		glPushMatrix();
		
		glTranslatef(model->meshframes[d]->px, model->meshframes[d]->py, model->meshframes[d]->pz);
		glRotatef(model->meshframes[d]->rx, 1, 0, 0);
		glRotatef(model->meshframes[d]->ry, 0, 1, 0);
		glRotatef(model->meshframes[d]->rz, 0, 0, 1);
		glScalef(model->meshframes[d]->sx, model->meshframes[d]->sy, model->meshframes[d]->sz);
		
		renderObj(model->meshframes[d]->mesh);
		
		glPopMatrix();
	}
}

/**
* Renders
**/
void RenderOpenGL::render()
{
	unsigned int i, j;
	AnimPlay * play;
	
	int x, y, w, h;	// for general use
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	
	
	// Calcs for rotations
	w = this->virt_width * 2;
	h = this->virt_height * 2;
	
	
	// Background
	/*glLoadIdentity();
	glTranslatef(this->virt_width / 2, this->virt_height / 2, 0);
	glRotatef(st->curr_player->angle, 0, 0, 1);
	glTranslatef(0 - w / 2, 0 - h / 2, 0);
	this->renderSprite(st->map->background, 0, 0, w, h);*/
	
	
	// Main map rotation
	glLoadIdentity();
	glTranslatef(this->virt_width / 2, this->virt_height / 2, 0);
	glRotatef(st->curr_player->angle, 0, 0, 1);
	glTranslatef(0 - st->curr_player->x, 0 - st->curr_player->y, 0);
	glTranslatef(0, 0, 500);
	
	
	// Set up lights
	GLfloat LightAmbient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat LightDiffuse[] = { 1.0f, 1.0f, 1.0f, 0.3f };
	GLfloat LightPosition[] = { 1000.0f, 1000.0f, 150.0f, 0 };
	
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
	glEnable(GL_LIGHT0);
	
	
	// Render map
	for (i = 0; i < st->map->areas.size(); i++) {
		Area * a = st->map->areas[i];
		
		glTranslatef(0, 0, 1);
		
		glPushMatrix();
		
		x = a->x + a->width / 2;
		y = a->y + a->height / 2;
		
		glTranslatef(x, y, 0);
		glRotatef(0 - a->angle, 0, 0, 1);
		glTranslatef(0 - x, 0 - y, 0);
		
		glBindTexture(GL_TEXTURE_2D, a->type->surf->pixels);
 		
 		float texw = 1.0;
 		float texh = 1.0;
 		if (! a->type->stretch) {
 			texw = ((float)a->width) / ((float)a->type->surf->w);
 			texh = ((float)a->height) / ((float)a->type->surf->h);
 		}
 		
		glBegin(GL_QUADS);
			// Bottom-left vertex (corner)
			glTexCoord2f( 0.0, texh );
			glNormal3f(a->x, a->y, 1);
			glVertex3i( a->x, a->y + a->height, 0 );
			
			// Bottom-right vertex (corner)
			glTexCoord2f( texw, texh );
			glNormal3f(a->x, a->y, 1);
			glVertex3i( a->x + a->width, a->y + a->height, 0 );
			
			// Top-right vertex (corner)
			glTexCoord2f( texw, 0.0 );
			glNormal3f(a->x, a->y, 1);
			glVertex3i( a->x + a->width, a->y, 0 );
			
			// Top-left vertex (corner)
			glTexCoord2f( 0.0, 0.0 );
			glNormal3f(a->x, a->y, 1);
			glVertex3i( a->x, a->y, 0 );
		glEnd();
		
		glPopMatrix();
	}
	
	glTranslatef(0, 0, 10);
	
	// Entities
	//std::sort(st->entities.begin(), st->entities.end(), ZIndexPredicate);
	for (i = 0; i < st->entities.size(); i++) {
		Entity *e = st->entities.at(i);
		
		
		glPushMatrix();
		
		glTranslatef(e->x, e->y, e->z);
		glRotatef(0 - e->angle, 0, 0, 1);
		
		
		AnimPlay * list [SPRITE_LIST_LEN] = {NULL, NULL, NULL, NULL};
		e->getAnimModel(list);
		
		for (j = 0; j < SPRITE_LIST_LEN; j++) {
			play = list[j];
			if (play == NULL) break;
			
			renderAnimPlay(play);
		}
		
		
		glPopMatrix();
	}
	
	// HUD
	glLoadIdentity();
	glTranslatef(0, 0, 900);
	glDisable(GL_LIGHTING);
	st->hud->render(this);
	
	SDL_GL_SwapBuffers();
}


