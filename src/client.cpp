#include <iostream>
#include <SDL.h>
#include "rage.h"


using namespace std;



int main (int argc, char ** argv) {
	
	Render * render = new RenderSDL();
	render->setScreenSize(1000, 600, false);
	
	if (! loadAllAreaTypes(render)) {
		cerr << "Unable to load areatypes datafile.\n";
		exit(1);
	}
	
	if (! loadAllUnitClasses(render)) {
		cerr << "Unable to load unitclasses datafile.\n";
		exit(1);
	}
	
	if (! loadAllParticleTypes(render)) {
		cerr << "Unable to load particletypes datafile.\n";
		exit(1);
	}
	
	if (! loadAllWeaponTypes(render)) {
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
	
	p = new Player(getUnitClassByID(0), st);
	st->addUnit(p);
	p->x = 50;
	p->y = 50;
	
	p = new Player(getUnitClassByID(0), st);
	st->addUnit(p);
	p->x = 100;
	p->y = 100;
	
	Map *m = new Map();
	m->load("arena");
	st->map = m;
	
	
	gameLoop(st, render);
	
	
	exit(0);
}




