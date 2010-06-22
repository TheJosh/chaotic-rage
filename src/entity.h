#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


enum EntityClass {
	UNIT = 1,
	PARTICLE = 2,
	PGENERATOR = 3,
};


class Entity
{
	public:
		virtual EntityClass klass() const = 0;
		
	public:
		bool del;
		int x;
		int y;
		int height;		// for proper z-indexing
		
	protected:
		GameState* st;
		
	protected:
		Entity(GameState *st);
	
	public:
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
		
		
	public:
		GameState * getGameState();
};
