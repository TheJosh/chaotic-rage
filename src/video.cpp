#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


void render(GameState *st, SDL_Surface *screen)
{
	unsigned int i;
	SDL_Surface *surf;
	
	// Dirt layer
	surf = st->map->ground;
	SDL_BlitSurface(surf, NULL, screen, 0);
	
	// Wall layer
	surf = st->map->walls;
	SDL_BlitSurface(surf, NULL, screen, 0);
	
	// Units
	for (i = 0; i < st->units.size(); i++) {
		Unit *e = (Unit*) st->units.at(i);
		
		SDL_Rect r;
		r.x = e->x;
		r.y = e->y;
		
		SDL_BlitSurface(e->getSprite(), NULL, screen, &r);
	}
	
	// Particles
	for (i = 0; i < st->particles.size(); i++) {
		Particle *e = (Particle*) st->particles.at(i);
		
		SDL_Rect r;
		r.x = e->x;
		r.y = e->y;
		
		SDL_BlitSurface(e->getSprite(), NULL, screen, &r);
	}
	
	SDL_Flip(screen);
}


