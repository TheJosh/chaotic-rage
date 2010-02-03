#include <iostream>
#include <SDL.h>
#pragma once

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
		virtual SDL_Surface* getSprite() = 0;
		virtual void update(int msdelta) = 0;
};
