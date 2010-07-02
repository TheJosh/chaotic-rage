#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class Player : public Unit
{
	public:
		static const int KEY_UP = 0;
		static const int KEY_LEFT = 1;
		static const int KEY_DOWN = 2;
		static const int KEY_RIGHT = 3;
		
	private:
		bool key[3];
		
	public:
		Player(UnitClass *uc, GameState *st);
		~Player();
		
	public:
		void keyPress(int idx);
		void keyRelease(int idx);
		void angleFromMouse(int x, int y);
		
	public:
		virtual void update(int delta);
};
