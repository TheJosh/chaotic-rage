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


RenderSDL::RenderSDL()
{
	this->screen = NULL;
}

RenderSDL::~RenderSDL()
{

}


void RenderSDL::setScreenSize(int width, int height, bool fullscreen)
{
	this->screen = SDL_SetVideoMode(width, height, 32, SDL_SWSURFACE);
	
	if (screen == NULL) {
		fprintf(stderr, "Unable to set %ix%i video: %s\n", width, height, SDL_GetError());
		exit(1);
	}
}


void RenderSDL::render(GameState *st)
{
	unsigned int i;
	SDL_Surface *surf;
	
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
		
		SDL_Surface *surf = e->getSprite();
		if (surf == NULL) continue;
		
		SDL_BlitSurface(surf, NULL, this->screen, &r);
	}
	
	st->hud->render(this->screen);
	
	SDL_Flip(this->screen);
}


