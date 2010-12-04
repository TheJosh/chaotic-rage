// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <algorithm>
#include <SDL.h>
#include "rage.h"

using namespace std;


GameLogic::GameLogic(GameState *st)
{
	st->logic = this;
	this->st = st;
}

void GameLogic::handleEvent(Event * ev)
{
	if (ev->type == GAME_STARTED) {
		
		Mod * mod = st->getMod(0);
		Map * map = st->map;
		
		Player *p = new Player(mod->getUnitClass(0), st);
		st->addUnit(p);
		st->curr_player = p;
		
		p->pickupWeapon(mod->getWeaponType(1));
		p->pickupWeapon(mod->getWeaponType(2));
		
		Zone *z = map->getSpawnZone(FACTION_INDIVIDUAL);
		if (z == NULL) {
			cerr << "Map does not have any spawnpoints\n";
			exit(1);
		}
		p->x = z->getRandomX();
		p->y = z->getRandomY();
		
	}
}
