#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


Entity::Entity(GameState *st)
{
	this->del = false;
	this->x = 0;
	this->y = 0;
	this->st = st;
}

Entity::~Entity()
{
}
