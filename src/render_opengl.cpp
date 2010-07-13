// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <algorithm>
#include <GL/gl.h>
#include <GL/glu.h>
#include "rage.h"

using namespace std;


static bool ZIndexPredicate(const Entity * e1, const Entity * e2)
{
	int e1_y = e1->y + (e1->height / 2);
	int e2_y = e2->y + (e2->height / 2);
	
	return e1_y < e2_y;
}


RenderOpenGL::RenderOpenGL() : Render()
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
	// TODO: we need to keep track of all textures
	// because when this method runs again, we need to re-load everything
	// back into OpenGL
	// Priority: Medium
	
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	
	this->screen = SDL_SetVideoMode(width, height, 32, SDL_OPENGL);
	if (screen == NULL) {
		fprintf(stderr, "Unable to set %ix%i video: %s\n", width, height, SDL_GetError());
		exit(1);
	}
	
	SDL_WM_SetCaption("Chaotic Rage", "Chaotic Rage");
	SDL_ShowCursor(SDL_DISABLE);
	
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	glViewport(0, 0, width, height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
	
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
	
	surf = SDL_LoadBMP_RW(rw, 0);
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
	
	// Leak!
	surf = SDL_ConvertSurface(surf, this->screen->format, SDL_SWSURFACE);
	
	// Colour-key the surface
	unsigned int colourkey = SDL_MapRGB(surf->format, 255, 0, 255);
	unsigned int trans = SDL_MapRGBA(surf->format, 0, 0, 0, 0);
	
	for (int y = 0; y < surf->h; y++) {
		for (int x = 0; x < surf->w; x++) {
			unsigned int pix = ((unsigned int*)surf->pixels)[y*(surf->pitch/sizeof(unsigned int)) + x];
			if (pix == colourkey) {
				((unsigned int*)surf->pixels)[y*(surf->pitch/sizeof(unsigned int)) + x] = trans;
			}
		}
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
	
	glTexImage2D(GL_TEXTURE_2D, 0, num_colors, surf->w, surf->h, 0, texture_format, GL_UNSIGNED_BYTE, surf->pixels);
	
	//SDL_FreeSurface(surf);
	
	return sprite;
}


/**
* Renders a sprite.
* Should only be used if the the caller was called by this classes 'Render' function.
**/
void RenderOpenGL::renderSprite(SpritePtr sprite, int x, int y)
{
	glBindTexture(GL_TEXTURE_2D, sprite->pixels);
 	
	glBegin(GL_QUADS);
		// Bottom-left vertex (corner)
		glTexCoord2i( 0, 1 );
		glVertex2i( x, y + sprite->w );
		
		// Bottom-right vertex (corner)
		glTexCoord2i( 1, 1 );
		glVertex2i( x + sprite->w, y + sprite->h );
		
		// Top-right vertex (corner)
		glTexCoord2i( 1, 0 );
		glVertex2i( x + sprite->w, y );
		
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
	
	glTexImage2D(GL_TEXTURE_2D, 0, 4, surf->w, surf->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, surf->pixels);
	
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
void RenderOpenGL::render(GameState *st)
{
	static int angle = 0.0;
	angle +=  360.0 / 10.0;

	unsigned int i;
	SpritePtr sprite;
	
	glClear(GL_COLOR_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(this->screen->w / 2, this->screen->h / 2, 0);
	glRotatef(st->curr_player->angle, 0, 0, 1);
	glTranslatef(0 - st->curr_player->x, 0 - st->curr_player->y, 0);
	
	// TODO: render backgrounds
	// Priority: High
	// Dirt layer
	this->renderSprite(st->map->ground, 0, 0);
	
	// Wall layer
	//this->renderSprite(st->map->walls, 0, 0);
	
	// Entities
	std::sort(st->entities.begin(), st->entities.end(), ZIndexPredicate);
	for (i = 0; i < st->entities.size(); i++) {
		Entity *e = st->entities.at(i);
		
		sprite = e->getSprite();
		if (sprite == NULL) continue;
		
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
	
	glLoadIdentity();
	st->hud->render(this);
	
	SDL_GL_SwapBuffers();
}


