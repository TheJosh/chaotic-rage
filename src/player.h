// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

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
		int prev_x;
		
	public:
		Player(UnitType *uc, GameState *st);
		virtual ~Player();
		
	public:
		void keyPress(int idx);
		void keyRelease(int idx);
		void angleFromMouse(int x, int y);
		
	public:
		virtual void update(int delta);
		virtual int takeDamage(int damage);
};
