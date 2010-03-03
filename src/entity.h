#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class Entity
{
	public:
		bool del;
		int x;
		int y;
		
	protected:
		GameState* st;
		
	protected:
		Entity(GameState *st);
		~Entity();
		
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
