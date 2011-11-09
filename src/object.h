// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class Object : public Entity
{
	public:
		virtual EntityClass klass() const { return OBJECT; }
		
	public:
		ObjectType* ot;
		int health;
		
	protected:
		AnimPlay * anim;
		CollideBox * cb;
	public:
		Object(ObjectType *pt, GameState *st, float x, float y, float z);
		virtual ~Object();
		
	public:
		virtual AnimPlay* getAnimModel();
		virtual Sound* getSound();
		virtual void update(int delta);
		virtual void hasBeenHit(CollideBox * ours, CollideBox * theirs);
		
	public:
		void takeDamage(int damage);
};
