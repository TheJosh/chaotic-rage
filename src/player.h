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
		static const int KEY_FIRE = 2;
		
	private:
		bool key[2];
		
	public:
		Player(UnitClass *uc, GameState *st);
		~Player();
		
	public:
		void keyPress(int idx);
		void keyRelease(int idx);
		void angleFromMouse(int x, int y);
		
	public:
		virtual SDL_Surface* getSprite();
		virtual void update(int delta);
};
