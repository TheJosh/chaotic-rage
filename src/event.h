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
	HIT_WALL       = 1,  // particles
	HIT_UNIT       = 2,  // particles
	HIT_PARTICLE   = 3,  // units
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


