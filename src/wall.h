// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class Wall : public Entity
{
	public:
		virtual EntityClass klass() const { return WALL; }
		
	public:
		WallType* wt;
		int health;
		
	protected:
		AnimPlay * anim;
		
	public:
		Wall(WallType *pt, GameState *st, float x, float y, float z, float angle);
		virtual ~Wall();
		
	public:
		virtual AnimPlay* getAnimModel();
		virtual Sound* getSound();
		virtual void update(int delta);
		
	public:
		void takeDamage(int damage);
};
