#include <iostream>
#include <SDL.h>
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
	cout << "Unit::getSprite\n";
	return NULL;
}

void Unit::update(int msdelta)
{
	this->x += 30;
	cout << "Unit::update\n";
}
