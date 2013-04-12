// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


/**
* Some weapons need to create entities which exist in the game world
* for a period of time. Examples include rockets and mines
**/
class AmmoRound : public Entity
{
	public:
		virtual EntityClass klass() const { return NONE; }
		
	public:
		WeaponType *wt;
		AnimPlay *anim;
		void* data;
		
	public:
		AmmoRound(GameState* st, btTransform& xform, WeaponType* wt, AssimpModel* model);
		virtual ~AmmoRound();
		
	public:
		virtual AnimPlay* getAnimModel();
		virtual Sound* getSound();
		virtual void update(int delta);
};

