// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "unit.h"


class GameState;
class UnitType;

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
		static const int KEY_MELEE = 7;
		static const int KEY_SPECIAL = 8;
		
	public:
		bool key[16];
		bool lkey[16];
		float mouse_angle;
		float vertical_angle;
		bool walking;

	public:
		Player(UnitType *uc, GameState *st, float x, float y, float z);
		virtual ~Player();
		
	public:
		void keyPress(int idx);
		void keyRelease(int idx);
		void handleKeyChange();
		void angleFromMouse(int x, int y, int delta);
		void setKeys(Uint8 bitfield);		// TODO: Use 16-bits
		Uint8 packKeys();					// TODO: Use 16-bits	

	public:
		virtual void update(int delta);
		virtual int takeDamage(float damage);
};
