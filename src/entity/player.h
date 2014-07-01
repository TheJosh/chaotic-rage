// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "../rage.h"
#include "unit.h"

class GameState;
class UnitType;
class Vehicle;


class Player : public Unit
{
	public:
		enum Key {
			KEY_UP,
			KEY_LEFT,
			KEY_DOWN,
			KEY_RIGHT,
			KEY_FIRE,
			KEY_USE,
			KEY_LIFT,
			KEY_MELEE,
			KEY_SPECIAL,
		};

	public:
		bool key[16];
		bool lkey[16];
		float mouse_angle;
		float vertical_angle;
		bool walking;
		Vehicle* drive_old;

	public:
		Player(UnitType *uc, GameState *st, float x, float y, float z, Faction fac, int slot);
		virtual ~Player();

	public:
		void keyPress(Key idx);
		void keyRelease(Key idx);
		void handleKeyChange();
		void angleFromMouse(int x, int y, int delta);
		void setKeys(Uint8 bitfield);		// TODO: Use 16-bits
		Uint8 packKeys();					// TODO: Use 16-bits

	public:
		virtual void update(int delta);
		virtual int takeDamage(float damage);
};
