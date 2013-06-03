// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "../rage.h"

using namespace std;


class Pickup : public Entity
{
	public:
		virtual EntityClass klass() const { return PICKUP; }
		
	protected:
		PickupType* pt;
		AnimPlay * anim;
		
	public:
		Pickup(PickupType *pt, GameState *st, float x, float y, float z);
		virtual ~Pickup();
		
	public:
		virtual AnimPlay* getAnimModel();
		virtual Sound* getSound();
		virtual void update(int delta);
};

