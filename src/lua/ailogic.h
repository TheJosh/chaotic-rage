// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "../rage.h"
#include "luatimer.h"

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}

using namespace std;


/**
* Logic for the game - like spawning units, etc
**/
class AILogic
{
	public:
		Unit *u;
		lua_State *L;
		GameState *st;
		vector<LuaTimer*> timers;
		
	public:
		AILogic(Unit *u);
		~AILogic();
		
	private:
		void ActiveLuaState();
		
	public:
		/**
		* Executes a script.
		* The script execution typically binds functions to events.
		* Returns false if there is an error
		**/
		bool execScript(string code);
		
		/**
		* Basically just provides timer ticks
		**/
		void update(int delta);
		
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


