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
* Handlers return TRUE if they handled the event and FALSE if they didn't
**/
class MouseEventHandler {
	public:
		MouseEventHandler() {}
		virtual ~MouseEventHandler() {}
		virtual bool onMouseMove(Uint16 x, Uint16 y) { return false; }
		virtual bool onMouseDown(Uint8 button, Uint16 x, Uint16 y) { return false; }
		virtual bool onMouseUp(Uint8 button, Uint16 x, Uint16 y) { return false; }
};


/**
* Allows GameLogic code to handle keyboard events
* Handlers return TRUE if they handled the event and FALSE if they didn't
**/
class KeyboardEventHandler {
	public:
		KeyboardEventHandler() {}
		virtual ~KeyboardEventHandler() {}
		virtual bool onKeyDown(Uint16 keycode) { return false; }
		virtual bool onKeyUp(Uint16 keycode) { return false; }
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
		KeyboardEventHandler* keyboard_events;

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
		* Handle a mouse-move event
		**/
		bool onMouseMove(Uint16 x, Uint16 y)
		{
			return this->mouse_events->onMouseMove(x, y);
		}

		/**
		* Handle a mouse-down event
		**/
		bool onMouseDown(Uint8 button, Uint16 x, Uint16 y)
		{
			return this->mouse_events->onMouseDown(button, x, y);
		}

		/**
		* Handle a mouse-up event
		**/
		bool onMouseUp(Uint8 button, Uint16 x, Uint16 y)
		{
			return this->mouse_events->onMouseUp(button, x, y);
		}


		/**
		* Does the game logic want to receive mouse events?
		**/
		bool keyboardCaptured()
		{
			return (this->keyboard_events != NULL);
		}

		/**
		* Handle a key-down event
		**/
		bool onKeyDown(Uint16 keycode)
		{
			return this->keyboard_events->onKeyDown(keycode);
		}

		/**
		* Handle a key-up event
		**/
		bool onKeyUp(Uint16 keycode)
		{
			return this->keyboard_events->onKeyUp(keycode);
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


