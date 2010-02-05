#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"

using namespace std;


Unit::Unit() {
	return;
}

Unit::~Unit() {
	
	return;
}

SDL_Surface* Unit::getSprite()
{
	return NULL;
}

void Unit::update(int usdelta)
{
	this->x += usdelta / 1200;
	
	if (this->x >= 600) this->x = 0;
}
