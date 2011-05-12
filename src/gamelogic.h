// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}

using namespace std;


/**
* Logic for the game - like spawning units, etc
**/
class GameLogic : public EventListener
{
	public:
		GameState *st;
		Map *map;
		Mod *mod;
		vector<int> binds_start;

	public:
		GameLogic(GameState *st);
		~GameLogic();
		
	public:
		virtual void handleEvent(Event * ev);
		void update(int delta);
		
	public:
		/**
		* Executes a script.
		* The script execution basically binds functions to events.
		* Returns false if there is an error
		**/
		bool execScript(string code);
		
		/**
		* Raise a 'Gamestart' Lua event
		**/
		void raiseGamestart();
		
	public:
		int bind_gamestart(lua_State *L);
		
		
	// When this gets switched to use lua, this will all leave
	private:
		int spawn_timer;
		int num_zomb;
		int num_wanted_zomb;
		int num_killed;
		
		int player_spawn;
};
