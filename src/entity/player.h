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
			KEY_ZOOM,
		};

	public:
		bool key[16];
		bool lkey[16];
		float mouse_angle;
		float vertical_angle;
		bool walking;
		Vehicle* drive_old;
		unsigned int death_time;

	public:
		Player(UnitType *uc, GameState *st, Faction fac, int slot, float x, float z);
		Player(UnitType *uc, GameState *st, Faction fac, int slot, float x, float y, float z);
		Player(UnitType *uc, GameState *st, Faction fac, int slot, btTransform & loc);
		virtual ~Player();

	public:
		void resetKeyPress(bool resetHistory = false);
		void keyPress(Key idx);
		void keyRelease(Key idx);
		void handleKeyChange();
		void angleFromMouse(int x, int y, int delta);
		void setKeys(Uint16 bitfield);
		Uint16 packKeys();
		bool isDying();
		float dyingPercent();
		void actualDeath();

	public:
		virtual void update(int delta);
		virtual void die();
};
