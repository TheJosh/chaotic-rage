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
	
	if (! loadAllWeaponTypes()) {
		cerr << "Unable to load weapontypes datafile.\n";
		exit(1);
	}
	
	
	seedRandom();
	
	
	GameState *st = new GameState();
	
	Player *p = new Player(getUnitClassByID(0), st);
	st->addUnit(p);
	st->curr_player = p;
	
	p->pickupWeapon(getWeaponTypeByID(1));
	p->pickupWeapon(getWeaponTypeByID(2));
	
	Map *m = new Map();
	m->load("arena");
	st->map = m;
	
	
	gameLoop(st, screen);
	
	
	exit(0);
}




