// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


Entity::Entity(GameState *st)
{
	this->del = false;
	this->x = 0;
	this->y = 0;
	this->z = 0;
	this->st = st;
	this->radius = 30;		// Todo: Dynamic based on entity type
	this->collide = true;
}

Entity::~Entity()
{
}

GameState * Entity::getGameState()
{
	return this->st;
}


void Entity::hasDied()
{
	this->del = true;
	
	list<Entity *>::iterator it;
	for (it = this->hits.begin(); it != this->hits.end(); it++) {
		Entity *e = (*it);
		e->hits.remove(this);
	}
}


void Entity::hasHit(Entity *e)
{
	this->hits.push_back(e);
}

bool Entity::inContactWith(EntityClass klass)
{
	list<Entity *>::iterator it;
	for (it = this->hits.begin(); it != this->hits.end(); it++) {
		Entity *e = (*it);
		if (e->klass() == klass) return true;
	}
	return false;
}


