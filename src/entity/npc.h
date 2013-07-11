// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "../rage.h"

using namespace std;


class NPC : public Unit
{
	private:
		AILogic *logic;
		
	public:
		NPC(UnitType *uc, GameState *st, float x, float y, float z, AIType *ai);
		virtual ~NPC();
		
	private:
		int vals[4];
		bool currently_hit;
		unsigned int idle_sound_time;
		
	public:
		virtual void update(int delta);
		virtual int takeDamage(int damage);
};
