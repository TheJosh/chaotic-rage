#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class Player : public Unit
{
	public:
		static const int KEY_FWD = 0;
		static const int KEY_REV = 1;
		
	private:
		SDL_Surface** sprites;
		int curr_sprite;
		bool key[1];
		
	public:
		Player();
		~Player();
		
	public:
		void keyPress(int idx);
		void keyRelease(int idx);
		
	public:
		virtual SDL_Surface* getSprite();
		virtual void update(int usdelta);
};
