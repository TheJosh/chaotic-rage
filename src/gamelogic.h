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


class Timer;


/**
* Logic for the game - like spawning units, etc
**/
class GameLogic : public EventListener
{
	public:
		GameState *st;
		Map *map;
		Mod *mod;
		vector<int> binds_gamestart;
		vector<int> binds_unitdied;
		vector<Timer*> timers;
		
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
		
		/**
		* Raise a 'Unitdied' Lua event
		**/
		void raiseUnitdied();
		
		
	// When this gets switched to use lua, this will all leave
	private:
		int spawn_timer;
		int num_zomb;
		int num_wanted_zomb;
		int num_killed;
		
		int player_spawn;
};


class Timer
{
	public:
		unsigned int due;			// On or after this time, the function will be called
		unsigned int lua_func;		// The Lua function to call
		unsigned int interval;		// set to 0 for a one-off timer
};


