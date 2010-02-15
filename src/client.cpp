#include <iostream>
#include <SDL.h>
#include "rage.h"


using namespace std;



int main (int argc, char ** argv) {
	SDL_Surface *screen = SDL_SetVideoMode(800, 800, 32, SDL_SWSURFACE);
	
	if (screen == NULL) {
		fprintf(stderr, "Unable to set %ix%i video: %s\n", 800, 800, SDL_GetError());
		exit(1);
	}
	
	
	loadAllAreaTypes();
	
	
	GameState *st = new GameState();
	
	Player *p = new Player();
	st->curr_player = p;
	st->addUnit(p);
	
	Map *m = new Map();
	m->load("arena");
	st->map = m;
	
	
	gameLoop(st, screen);
	
	
	exit(0);
}




