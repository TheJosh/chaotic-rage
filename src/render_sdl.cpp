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
}


/**
* Internal sprite loading from a SDL_RWops
**/
SpritePtr RenderSDL::int_loadSprite(SDL_RWops *rw, string filename)
{
	SpritePtr sprite;
	Uint32 colourkey;
	
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
	
	colourkey = SDL_MapRGB(sprite->format, 255, 0, 255);
	SDL_SetColorKey(sprite, SDL_SRCCOLORKEY, colourkey);
	
	return sprite;
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
	SDL_BlitSurface(surf, NULL, this->screen, 0);
	
	// Wall layer
	surf = st->map->walls;
	SDL_BlitSurface(surf, NULL, this->screen, 0);
	
	// Entities
	std::sort(st->entities.begin(), st->entities.end(), ZIndexPredicate);
	for (i = 0; i < st->entities.size(); i++) {
		Entity *e = (Unit*) st->entities.at(i);
		
		SDL_Rect r;
		r.x = e->x;
		r.y = e->y;
		
		SpritePtr surf = e->getSprite();
		if (surf == NULL) continue;
		
		SDL_BlitSurface(surf, NULL, this->screen, &r);
	}
	
	st->hud->render(this->screen);
	
	SDL_Flip(this->screen);
}


