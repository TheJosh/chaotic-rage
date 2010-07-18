// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include "rage.h"


using namespace std;



int main (int argc, char ** argv) {
	
	SDL_Init(SDL_INIT_VIDEO);
	
	GameState *st = new GameState();
	
	new RenderOpenGL(st);
	new AudioSDLMixer(st);
	
	Render * render = st->render;
	render->setScreenSize(700, 700, false);
	
	
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
	
	
	Map *m = new Map();
	m->load("arena", render);
	st->map = m;
	
	Player *p = new Player(getUnitClassByID(0), st);
	st->addUnit(p);
	st->curr_player = p;
	
	p->pickupWeapon(getWeaponTypeByID(1));
	p->pickupWeapon(getWeaponTypeByID(2));
	
	Zone *z = m->getSpawnZone(FACTION_INDIVIDUAL);
	if (z == NULL) {
		cerr << "Map does not have any spawnpoints\n";
		exit(1);
	}
	p->x = z->getRandomX();
	p->y = z->getRandomY();
	
	p = new Player(getUnitClassByID(0), st);
	st->addUnit(p);
	p->x = 50;
	p->y = 50;
	
	p = new Player(getUnitClassByID(0), st);
	st->addUnit(p);
	p->x = 100;
	p->y = 100;
	

	
	
	gameLoop(st, render);
	
	
	exit(0);
}




