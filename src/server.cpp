// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include "rage.h"


using namespace std;



int main (int argc, char ** argv) {
	
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	
	seedRandom();
	
	GameState *st = new GameState();
	
	new RenderNull(st);
	new AudioSDLMixer(st);
	
	st->render->setScreenSize(900, 900, false);
	
	Mod * mod = new Mod(st, "data/cr/");
	
	if (! mod->load()) {
		cerr << "Unable to load datafile.\n";
		exit(1);
	}
	
	
	Map *m = new Map(st);
	m->load("arena", st->render);
	st->map = m;
	
	Player *p = new Player(mod->getUnitClass(0), st);
	st->addUnit(p);
	st->curr_player = p;
	
	p->pickupWeapon(mod->getWeaponType(1));
	p->pickupWeapon(mod->getWeaponType(2));
	
	Zone *z = m->getSpawnZone(FACTION_INDIVIDUAL);
	if (z == NULL) {
		cerr << "Map does not have any spawnpoints\n";
		exit(1);
	}
	p->x = z->getRandomX();
	p->y = z->getRandomY();
	
	
	gameLoop(st, st->render);
	
	
	exit(0);
}




