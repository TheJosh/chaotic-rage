// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <algorithm>
#include <SDL.h>
#include "rage.h"

using namespace std;


static bool ZIndexPredicate(const Entity * e1, const Entity * e2)
{
	int e1_y = e1->y + (e1->height / 2);
	int e2_y = e2->y + (e2->height / 2);
	
	return e1_y < e2_y;
}


RenderSDL::RenderSDL() : Render()
{
	this->screen = NULL;
}

RenderSDL::~RenderSDL()
{
}


/**
* Sets the screen size
**/
void RenderSDL::setScreenSize(int width, int height, bool fullscreen)
{
	this->screen = SDL_SetVideoMode(width, height, 32, SDL_SWSURFACE);
	
	if (screen == NULL) {
		fprintf(stderr, "Unable to set %ix%i video: %s\n", width, height, SDL_GetError());
		exit(1);
	}
	
	this->colourkey = SDL_MapRGB(this->screen->format, 255, 0, 255);
}


/**
* Internal sprite loading from a SDL_RWops
**/
SpritePtr RenderSDL::int_loadSprite(SDL_RWops *rw, string filename)
{
	SDL_Surface * sprite;
	
	sprite = SDL_LoadBMP_RW(rw, 0);
	
	if (sprite == NULL) {
		fprintf(stderr, "Couldn't load sprite '%s'.\n", filename.c_str());
		load_err = true;
		return NULL;
	}
	
	SDL_WM_SetCaption("Chaotic Rage", "Chaotic Rage");
	
	if (sprite->format->BitsPerPixel != 24) {
		DEBUG("Bitmap '%s' not in 24-bit colour; may have problem with colour-key\n", filename.c_str());
	}
	
	sprite = SDL_DisplayFormat(sprite); // leak !!
	
	SDL_SetColorKey(sprite, SDL_SRCCOLORKEY, this->colourkey);
	
	return (SpritePtr) sprite;
}


/**
* Renders a sprite.
**/
void RenderSDL::renderSprite(SpritePtr sprite, int x, int y)
{
	SDL_Rect r;
	r.x = x;
	r.y = y;
	
	SDL_BlitSurface((SDL_Surface*) sprite, NULL, this->screen, &r);
}


/**
* Render a single frame of the wall animation.
**/
SpritePtr RenderSDL::renderMap(Map * map, int frame, bool wall)
{
	// Create
	SDL_Surface* surf = SDL_CreateRGBSurface(SDL_SWSURFACE, map->width, map->height, 32, 0,0,0,0);
	
	// Colour key for frame surfaces
	SDL_SetColorKey(surf, SDL_SRCCOLORKEY, this->colourkey);
	SDL_FillRect(surf, NULL, this->colourkey);
	
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
		SDL_Surface *areasurf = (SDL_Surface*)at->surf;
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
	
	return (SpritePtr) surf;
}


/**
* Clears all colour from a given pixel for a given sprite
**/
void RenderSDL::clearPixel(SpritePtr sprite, int x, int y)
{
	setPixel((SDL_Surface*) sprite, x, y, this->colourkey);
}

/**
* Free sprite memory
**/
void RenderSDL::freeSprite(SpritePtr sprite)
{
	SDL_FreeSurface((SDL_Surface*) sprite);
}

/**
* Returns sprite width
**/
int RenderSDL::getSpriteWidth(SpritePtr sprite)
{
	return ((SDL_Surface*)sprite)->w;
}

/**
* Returns sprite height
**/
int RenderSDL::getSpriteHeight(SpritePtr sprite)
{
	return ((SDL_Surface*)sprite)->h;
}

/**
* Renders
**/
void RenderSDL::render(GameState *st)
{
	unsigned int i;
	SpritePtr surf;
	
	// Dirt layer
	surf = st->map->ground;
	SDL_BlitSurface((SDL_Surface*) surf, NULL, this->screen, 0);
	
	// Wall layer
	surf = st->map->walls;
	SDL_BlitSurface((SDL_Surface*) surf, NULL, this->screen, 0);
	
	// Entities
	std::sort(st->entities.begin(), st->entities.end(), ZIndexPredicate);
	for (i = 0; i < st->entities.size(); i++) {
		Entity *e = (Unit*) st->entities.at(i);
		
		SDL_Rect r;
		r.x = e->x;
		r.y = e->y;
		
		SpritePtr surf = e->getSprite();
		if (surf == NULL) continue;
		
		SDL_BlitSurface((SDL_Surface*) surf, NULL, this->screen, &r);
	}
	
	st->hud->render(this);
	
	SDL_Flip(this->screen);
}


