// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <algorithm>
#include <SDL.h>
#include "rage.h"

using namespace std;


/**
* Basic set up
**/
GameLogic::GameLogic(GameState *st)
{
	st->logic = this;
	this->st = st;
	
	this->mod = st->getMod(0);
	this->map = st->map;
}


/**
* Handles all game events and determines what to actaully do to make this into an actual game
**/
void GameLogic::handleEvent(Event * ev)
{
	if (ev->type == GAME_STARTED) {
		st->curr_player = this->spawnPlayer(mod->getUnitClass(0), FACTION_TEAM1);
		
		for (int i = 0; i < 100; i++) {
			this->spawnNPC(mod->getUnitClass(1), FACTION_TEAM2);
		}
	}
}




/**
* Spawns a player into the specified faction
**/
Player * GameLogic::spawnPlayer(UnitClass *uc, Faction fac)
{
	Player *p;
	
	p = new Player(uc, st);
	st->addUnit(p);
	
	p->pickupWeapon(mod->getWeaponType(1));
	p->pickupWeapon(mod->getWeaponType(2));
	
	Zone *z = map->getSpawnZone(fac);
	if (z == NULL) {
		cerr << "Map does not have any spawnpoints\n";
		exit(1);
	}
	
	p->x = z->getRandomX();
	p->y = z->getRandomY();
	
	return p;
}

/**
* Spawns a player into the specified faction
**/
NPC * GameLogic::spawnNPC(UnitClass *uc, Faction fac)
{
	NPC *p;
	
	p = new NPC(uc, st);
	st->addUnit(p);
	
	p->pickupWeapon(mod->getWeaponType(1));
	p->pickupWeapon(mod->getWeaponType(2));
	
	Zone *z = map->getSpawnZone(fac);
	if (z == NULL) {
		cerr << "Map does not have any spawnpoints\n";
		exit(1);
	}
	
	p->x = z->getRandomX();
	p->y = z->getRandomY();
	
	return p;
}

