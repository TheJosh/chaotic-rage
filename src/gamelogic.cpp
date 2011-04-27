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
	this->map = st->curr_map;
}


/**
* Handles all game events and determines what to actaully do to make this into an actual game
**/
void GameLogic::handleEvent(Event * ev)
{
	if (ev->type == GAME_STARTED) {
		this->player_spawn = -1;
		
		this->spawn_timer = 0;
		this->num_zomb = 0;
		this->num_killed = 0;
		this->num_wanted_zomb = 100;
		
	} else if (ev->type == UNIT_DIED) {
		this->num_killed++;
		cout << "Num killed: " << this->num_killed << "\n";
	}
}


void GameLogic::update(int delta)
{
	spawn_timer += delta;
	if (this->num_zomb < this->num_wanted_zomb && this->spawn_timer > 250) {
		this->spawnNPC(mod->getUnitType(2), FACTION_TEAM2);
		this->num_zomb++;
		this->spawn_timer -= 250;
	}
	
	if (st->curr_player == NULL && this->player_spawn == -1) {
		// Player has died, show spawn menu
		st->hud->showSpawnMenu();
		this->player_spawn = st->game_time;
		
	} else if (st->curr_player == NULL && st->game_time - this->player_spawn > 1000) {
		// Spawn time over, create player
		st->hud->hideSpawnMenu();
		st->curr_player = this->spawnPlayer(mod->getUnitType(0), FACTION_TEAM1);
		this->player_spawn = -1;
		
	}
	
	if (this->num_zomb == this->num_killed) {
		cout << "All zombies are dead\n";
	}
}


/**
* Spawns a player into the specified faction
**/
Player * GameLogic::spawnPlayer(UnitType *uc, Faction fac)
{
	Player *p;
	
	p = new Player(uc, st);
	st->addUnit(p);
	
	p->pickupWeapon(mod->getWeaponType(0));
	p->pickupWeapon(mod->getWeaponType(1));
	p->pickupWeapon(mod->getWeaponType(2));
	
	Zone *z = map->getSpawnZone(fac);
	if (z == NULL) {
		cerr << "Map does not have any spawnpoints\n";
		exit(1);
	}
	
	p->x = z->getRandomX();
	p->y = z->getRandomY();
	p->fac = fac;
	
	return p;
}

/**
* Spawns a player into the specified faction
**/
NPC * GameLogic::spawnNPC(UnitType *uc, Faction fac)
{
	NPC *p;
	
	p = new NPC(uc, st);
	st->addUnit(p);
	
	p->pickupWeapon(mod->getWeaponType(1));
	
	Zone *z = map->getSpawnZone(fac);
	if (z == NULL) {
		cerr << "Map does not have any spawnpoints\n";
		exit(1);
	}
	
	p->x = z->getRandomX();
	p->y = z->getRandomY();
	p->fac = fac;
	
	return p;
}

