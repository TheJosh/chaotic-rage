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
		bool key[1];
		
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
