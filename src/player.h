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
		static const int KEY_FIRE = 4;
		static const int KEY_USE = 5;
		static const int KEY_LIFT = 6;
		static const int KEY_UNUSED3 = 7;
		
	public:
		bool key[7];

	public:
		Player(UnitType *uc, GameState *st);
		virtual ~Player();
		
	public:
		void keyPress(int idx);
		void keyRelease(int idx);
		void angleFromMouse(int x, int y, int delta);
		void setKeys(Uint8 bitfield);
		Uint8 packKeys();
		
	public:
		virtual void update(int delta);
		virtual int takeDamage(int damage);
		virtual void hasBeenHit(CollideBox * ours, CollideBox * theirs);

	private:
		bool use;
		void doUse();
		void doLift();
		Object* curr_obj;
		Object* lift_obj;
};
