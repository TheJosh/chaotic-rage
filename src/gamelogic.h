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
class GameLogic
{
	public:
		GameState *st;
		Map *map;
		Mod *mod;
		vector<Timer*> timers;
		
	public:
		UnitType * selected_unittype;

	public:
		GameLogic(GameState *st);
		~GameLogic();
		
	public:
		void update(int delta);
		
	public:
		/**
		* Executes a script.
		* The script execution basically binds functions to events.
		* Returns false if there is an error
		**/
		bool execScript(string code);
		
		/**
		* When the game starts
		**/
		void raise_gamestart();
		vector<int> binds_gamestart;
		
		/**
		* When a player joins
		**/
		void raise_playerjoin(int arg);
		vector<int> binds_playerjoin;
		
		/**
		* When a player has died
		**/
		void raise_playerdied();
		vector<int> binds_playerdied;
		
		/**
		* When an NPC has died
		**/
		void raise_npcdied();
		vector<int> binds_npcdied;
};


class Timer
{
	public:
		unsigned int due;			// On or after this time, the function will be called
		unsigned int lua_func;		// The Lua function to call
		unsigned int interval;		// set to 0 for a one-off timer
};


