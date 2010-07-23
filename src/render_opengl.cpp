// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <algorithm>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL_image.h>
#include "rage.h"

using namespace std;



static bool ZIndexPredicate(const Entity * e1, const Entity * e2)
{
	return e1->y < e2->y;
}


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
	
	// The 'virtual' size is 1000px high, with the proper width for your monitor.
	this->virt_height = 1000;
	this->virt_width = this->virt_height * (float)width / (float)height;
	
	SDL_WM_SetCaption("Chaotic Rage", "Chaotic Rage");
	SDL_ShowCursor(SDL_DISABLE);
	
	flags = IMG_INIT_PNG;
	int initted = IMG_Init(flags);
	if ((initted & flags) != flags) {
		fprintf(stderr, "Failed to init required png support!\n");
		exit(1);
	}
	
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	glViewport(0, 0, width, height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, this->virt_width, this->virt_height, 0.0f, -1.0f, 1.0f);
	
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


/**
* Render a single frame of the wall animation.
**/
SpritePtr RenderOpenGL::renderMap(Map * map, int frame, bool wall)
{
	// Create
	SDL_Surface* surf = SDL_CreateRGBSurface(SDL_SWSURFACE, map->width, map->height, 32, 0,0,0,0);
	
	Area *a;
	AreaType *at;
	unsigned int i;
	SDL_Rect dest;
	
	// Iterate through the areas
	for (i = 0; i < map->areas.size(); i++) {
		a = map->areas[i];
		at = a->type;
		
		//if (wall != a->type->wall) continue;
		
		if (wall and ! at->wall) continue;
		
		if (! wall and at->wall) {
			if (at->ground_type != NULL) {
				at = at->ground_type;
			} else {
				continue;
			}
		}
		
		// Transforms (either streches or tiles)
		SDL_Surface *areasurf = (SDL_Surface*)at->surf->orig;
		if (a->type->stretch)  {
			areasurf = rotozoomSurfaceXY(areasurf, 0, ((double)a->width) / ((double)areasurf->w), ((double)a->height) / ((double)areasurf->h), 0);
			if (areasurf == NULL) continue;
			
		} else {
			areasurf = (SDL_Surface*)tileSprite(areasurf, a->width, a->height);
			if (areasurf == NULL) continue;
		}
		
		// Rotates
		if (a->angle != 0)  {
			SDL_Surface* temp = areasurf;
			
			areasurf = rotozoomSurfaceXY(temp, a->angle, 1, 1, 0);
			SDL_FreeSurface(temp);
			if (areasurf == NULL) continue;
		}
		
		dest.x = a->x;
		dest.y = a->y;
		
		SDL_BlitSurface(areasurf, NULL, surf, &dest);
		SDL_FreeSurface(areasurf);
	}
	
	SpritePtr sprite = new struct sprite;
	sprite->w = map->width;
	sprite->h = map->height;
	sprite->orig = surf;
	
	// Open texture handle
	glGenTextures(1, &sprite->pixels);
	glBindTexture(GL_TEXTURE_2D, sprite->pixels);
	
	// Set stretching properties
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, surf->w, surf->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, surf->pixels);
	
	return sprite;
}


/**
* Clears all colour from a given pixel for a given sprite
**/
void RenderOpenGL::clearPixel(SpritePtr sprite, int x, int y)
{
	// TODO: code this
	// Priority: Low
	//setPixel((SDL_Surface*) sprite, x, y, this->colourkey);
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
* Renders
**/
void RenderOpenGL::render()
{
	unsigned int i, j;
	SpritePtr sprite;
	
	int x, y, w, h;	// for general use
	
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	
	// Calcs for rotations
	w = this->virt_width * 2;
	h = this->virt_height * 2;
	x = st->curr_player->x + st->curr_player->getWidth() / 2;
	y = st->curr_player->y + st->curr_player->getHeight() / 2;
	
	// Background
	glLoadIdentity();
	glTranslatef(this->virt_width / 2, this->virt_height / 2, 0);
	glRotatef(st->curr_player->angle, 0, 0, 1);
	glTranslatef(0 - w / 2, 0 - h / 2, 0);
	this->renderSprite(st->map->background, 0, 0, w, h);
	
	// Main map rotation
	glLoadIdentity();
	glTranslatef(this->virt_width / 2, this->virt_height / 2, 0);
	glRotatef(st->curr_player->angle, 0, 0, 1);
	glTranslatef(0 - x, 0 - y, 0);
	
	// Render backgrounds
	// Dirt layer
	this->renderSprite(st->map->ground, 0, 0);
	
	// Wall layer
	// TODO: Get working
	//this->renderSprite(st->map->walls, 0, 0);
	
	// Entities
	std::sort(st->entities.begin(), st->entities.end(), ZIndexPredicate);
	for (i = 0; i < st->entities.size(); i++) {
		Entity *e = st->entities.at(i);
		
		
		glPushMatrix();
		
		x = e->x + e->getWidth() / 2;
		y = e->y + e->getHeight() / 2;
		
		glTranslatef(x, y, 0);
		glRotatef(0 - e->angle, 0, 0, 1);
		glTranslatef(0 - x, 0 - y, 0);
		
		
		SpritePtr list [SPRITE_LIST_LEN] = {NULL, NULL, NULL, NULL};
		e->getSprite(list);
		
		for (j = 0; j < SPRITE_LIST_LEN; j++) {
			sprite = list[j];
			if (sprite == NULL) break;
			
			glBindTexture(GL_TEXTURE_2D, sprite->pixels);
	 		
			glBegin(GL_QUADS);
				// Bottom-left vertex (corner)
				glTexCoord2i( 0, 1 );
				glVertex2i( e->x, e->y + sprite->h );
				
				// Bottom-right vertex (corner)
				glTexCoord2i( 1, 1 );
				glVertex2i( e->x + sprite->w, e->y + sprite->h );
				
				// Top-right vertex (corner)
				glTexCoord2i( 1, 0 );
				glVertex2i( e->x + sprite->w, e->y );
				
				// Top-left vertex (corner)
				glTexCoord2i( 0, 0 );
				glVertex2i( e->x, e->y );
			glEnd();
		}
		
		glPopMatrix();
	}
	
	// HUD
	glLoadIdentity();
	st->hud->render(this);
	
	SDL_GL_SwapBuffers();
}


