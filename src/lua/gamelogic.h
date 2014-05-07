// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <SDL.h>
#include "../rage.h"
#include "luatimer.h"

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}

using namespace std;


/**
* Allows GameLogic code to handle mouse events
**/
class MouseEventHandler {
	public:
		virtual void onMouseDown(Uint8 button, Uint16 x, Uint16 y) {}
		virtual void onMouseUp(Uint8 button, Uint16 x, Uint16 y) {}
};


/**
* Logic for the game - like spawning units, etc
**/
class GameLogic
{
	public:
		GameState *st;
		Map *map;
		vector<LuaTimer*> timers;
		MouseEventHandler* mouse_events;

	public:
		UnitType * selected_unittype;

	public:
		GameLogic(GameState *st);
		~GameLogic();
		
	public:
		void update(int delta);
		
		/**
		* Does the game logic want to receive mouse events?
		**/
		bool mouseCaptured()
		{
			return (this->mouse_events != NULL);
		}

		/**
		* Handle a mouse-down event
		**/
		void onMouseDown(Uint8 button, Uint16 x, Uint16 y)
		{
			this->mouse_events->onMouseDown(button, x, y);
		}

		/**
		* Handle a mouse-up event
		**/
		void onMouseUp(Uint8 button, Uint16 x, Uint16 y)
		{
			this->mouse_events->onMouseUp(button, x, y);
		}

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
		* When a player leaves
		**/
		void raise_playerleave(int arg);
		vector<int> binds_playerleave;
		
		/**
		* When a player has died
		**/
		void raise_playerdied(int arg);
		vector<int> binds_playerdied;
		
		/**
		* When an NPC has died
		**/
		void raise_npcdied();
		vector<int> binds_npcdied;
};


