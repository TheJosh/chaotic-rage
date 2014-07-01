// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "../rage.h"
#include "unit.h"

class AILogic;
class AIType;
class GameState;
class UnitType;


using namespace std;


class NPC : public Unit
{
	private:
		AILogic *logic;

	public:
		NPC(UnitType *uc, GameState *st, float x, float y, float z, AIType *ai, Faction fac);
		virtual ~NPC();

	private:
		int vals[4];
		unsigned int idle_sound_time;

	public:
		virtual void update(int delta);
		virtual int takeDamage(float damage);
};
