// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "../rage.h"
#include "luatimer.h"

extern "C" {
	#include <lua.h>
}


class NPC;
class GameState;


/**
* Logic for the game - like spawning units, etc
**/
class AILogic
{
	friend class NPC;


	public:
		NPC *u;
		lua_State *lua;
		GameState *st;
		std::vector<LuaTimer*> timers;
		btVector3 dir;              // Direction of movement
		bool dir_flag;              // Set when direction has changed
		float speed;
		bool needEndFiring;

	public:
		AILogic(NPC *u);
		~AILogic();

	private:
		void ActiveLuaState();

	public:
		/**
		* Executes a script.
		* The script execution typically binds functions to events.
		* Returns false if there is an error
		**/
		bool execScript(std::string code);

		/**
		* Basically just provides timer ticks
		**/
		void update();

		btVector3& getDir() { return this->dir; }

};
