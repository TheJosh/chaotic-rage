// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <btBulletDynamicsCommon.h>
#include "rage.h"

using namespace std;


Entity::Entity(GameState *st)
{
	this->del = false;
	this->render = true;
	this->st = st;
	this->body = NULL;
}

Entity::~Entity()
{
}


/**
* Return the game state point - it is typically the same pointer everywhere.
**/
GameState * Entity::getGameState()
{
	return this->st;
}


/**
* Has this entity died?
**/
void Entity::hasDied()
{
	this->del = true;
}


