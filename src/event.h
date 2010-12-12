// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


/**
* Event types
**/
enum EventType {
	EVENT_NONE,
	
	UNIT_SPAWN,
	UNIT_WEP_PICKUP,
	UNIT_WEP_CHANGE,
	UNIT_WEP_BEG_FIRE,
	UNIT_WEP_END_FIRE,
	UNIT_PICKUP_OBJ,
	UNIT_DROP_OBJ,
	UNIT_DIED,
	UNIT_SCORE_KILL,
	UNIT_SCORE_ASSIST,
	UNIT_SCORE_SUISIDE,
	UNIT_ENTER_ZONE,
	UNIT_EXIT_ZONE,
	PART_HIT_WALL,
	PART_HIT_UNIT,
	GAME_STARTED,
	GAME_ENDED,
	ENTITY_HIT,
	ENTITY_UNHIT,
};


/**
* An in-game event
**/
class Event
{
	public:
		EventType type;
		Entity * e1;    // Primary entity, typically the giver
		Entity * e2;    // Secondary entity, typically the reciever
		int a1;
		int a2;
		
	public:
		Event();
		virtual ~Event() {};
};


/**
* Listens for events
**/
class EventListener
{
	public:
		/**
		* Handle an event
		* This object gets freed after given to this method, so will need cloning if being put onto a stack
		**/
		virtual void handleEvent(Event * ev) = 0;
};


void fireEvent (Event * ev);


