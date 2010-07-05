// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class EntityType
{
	public:
		vector<Action*>* actions;
		
	protected:
		EntityType();
		~EntityType();
		
	public:
		/**
		* Process the actions for an event
		**/
		void doActions(Entity * entity, ActionEvent event);
		
		/**
		* Spawns an entity of this type, and inserts it into the specified game state
		**/
		virtual Entity * spawn(GameState * st) = 0;
};



