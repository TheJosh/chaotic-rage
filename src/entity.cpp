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
}

Entity::~Entity()
{
}

GameState * Entity::getGameState()
{
	return this->st;
}
