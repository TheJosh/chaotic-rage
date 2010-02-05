#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


void render(GameState *st, SDL_Surface *screen)
{
	int i;
	
	SDL_FillRect(screen, NULL, 1000);
	
	for (i = 0; i < st->numUnits(); i++) {
		Player *e = (Player*) st->getUnit(i);
		
		SDL_Rect r;
		r.x = e->x;
		r.y = e->y;
		
		SDL_BlitSurface(e->getSprite(), NULL, screen, &r);
	}
	
	SDL_Flip(screen);
}
