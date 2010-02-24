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
	
	
	if (! loadAllAreaTypes()) {
		cerr << "Unable to load areatypes datafile.\n";
		exit(1);
	}
	
	if (! loadAllUnitClasses()) {
		cerr << "Unable to load unitclasses datafile.\n";
		exit(1);
	}
	
	if (! loadAllParticleTypes()) {
		cerr << "Unable to load particletypes datafile.\n";
		exit(1);
	}
	
	seedRandom();
	
	
	GameState *st = new GameState();
	
	Player *p = new Player(getUnitClassByID(0), st);
	st->addUnit(p);
	st->curr_player = p;
	
	Particle *pt;
	for (int i = 0; i < 150; i++) {
		pt = new Particle(getParticleTypeByID(0), st);
		pt->x = getRandom(50, 100);
		pt->y = getRandom(50, 100);
		st->addParticle(pt);
	}
	
	Map *m = new Map();
	m->load("arena");
	st->map = m;
	
	
	gameLoop(st, screen);
	
	
	exit(0);
}




