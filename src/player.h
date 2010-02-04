#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class Player : public Unit
{
	private:
		SDL_Surface* sprites;
		int curr_sprite;
		
	public:
		Player();
		~Player();
		
	public:
		virtual SDL_Surface* getSprite();
};
