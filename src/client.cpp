// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include "rage.h"


using namespace std;



int main (int argc, char ** argv) {
	
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	
	GameState *st = new GameState();
	
	new RenderOpenGL(st);
	new AudioSDLMixer(st);
	
	Render * render = st->render;
	render->setScreenSize(900, 900, false);
	
	Mod * mod = new Mod(st, "data/cr/");
	
	if (! mod->loadAreaTypes()) {
		cerr << "Unable to load area types.\n";
		exit(1);
	}
	
	if (! mod->loadUnitClasses()) {
		cerr << "Unable to load unit classes.\n";
		exit(1);
	}
	
	if (! mod->loadParticleTypes()) {
		cerr << "Unable to load particle types.\n";
		exit(1);
	}
	
	if (! mod->loadWeaponTypes()) {
		cerr << "Unable to load weapon types.\n";
		exit(1);
	}
	
	
	seedRandom();
	
	
	Map *m = new Map(st);
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




