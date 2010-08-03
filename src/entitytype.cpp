// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


EntityType::EntityType()
{
	this->actions = NULL;
}

EntityType::~EntityType()
{
	delete(this->actions);
}


/**
* Executes a set of actions for a given event for a given entity
**/
void EntityType::doActions(Event * ev)
{
	unsigned int i;
	
	if (this->actions == NULL) return;
	
	//GameState *st = ev->e1->getGameState();			// unused
	
	for (i = 0; i < this->actions->size(); i++) {
		Action *ac = this->actions->at(i);
		
		if (ac->event != ev->type) continue;
		
		switch (ac->type) {
			case ADD_PGENERATOR:
				{
					// TODO: Fix this
					// mod->getParticleGenType
					// need a pointer to the mod
					/*
					ParticleGenerator *pg = new ParticleGenerator(getParticleGenTypeByID(ac->args[0]), st);
					pg->x = ev->e1->x;
					pg->y = ev->e1->y;
					st->addParticleGenerator(pg);
					*/
				}
				break;
				
			default:
				break;
		}
	}
}
