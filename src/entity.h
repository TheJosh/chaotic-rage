#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class Entity
{
	public:
		int x;
		int y;
		int layer;
		
	protected:
		Entity();
		~Entity();
		
	public:
		;
		
	public:
		/**
		* Return a sprite to render this entity as
		**/
		virtual SDL_Surface* getSprite() = 0;
		
		/**
		* Update the entity. Time is provided in milliseconds (ms)
		**/
		virtual void update(int delta) = 0;
};
