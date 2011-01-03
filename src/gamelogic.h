// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


/**
* Logic for the game - like spawning units, etc
**/
class GameLogic : public EventListener
{
	public:
		GameState *st;
		
	private:
		Map *map;
		Mod *mod;
		
	public:
		GameLogic(GameState *st);
		
	public:
		virtual void handleEvent(Event * ev);
		void update(int delta);
		
	private:
		Player * spawnPlayer(UnitClass *uc, Faction fac);
		NPC * spawnNPC(UnitClass *uc, Faction fac);
		
		
	// When this gets switched to use lua, this will all leave
	private:
		int spawn_timer;
		int num_zomb;
		int num_wanted_zomb;
		int num_killed;
		
		int player_spawn;
};

