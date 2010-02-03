#include <iostream>
#include <SDL.h>
#include "entity.h"
#pragma once

using namespace std;


class Unit: public Entity
{
	public:
		string name;
		int angle;
		int speed;
		int health;
		
	public:
		Unit();
		~Unit();
		
	public:
		virtual SDL_Surface *getSprite();
		virtual void update(int msdelta);
};
