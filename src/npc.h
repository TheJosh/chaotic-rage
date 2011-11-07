// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class NPC : public Unit
{
	public:
		NPC(UnitType *uc, GameState *st, float x, float y, float z);
		virtual ~NPC();
		
	private:
		int vals[4];
		bool currently_hit;
		
	public:
		virtual void update(int delta);
		virtual void hasBeenHit(CollideBox * ours, CollideBox * theirs);
		virtual int takeDamage(int damage);
};
