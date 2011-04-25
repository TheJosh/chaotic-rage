// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class Wall: public Entity
{
	public:
		virtual EntityClass klass() const { return WALL; }
		
	public:
		WallType* wt;
		int health;
		
	protected:
		AnimPlay * anim;
		
	public:
		Wall(WallType *pt, GameState *st);
		virtual ~Wall();
		
	public:
		virtual void getSprite(SpritePtr list [SPRITE_LIST_LEN]);
		virtual void getAnimModel(AnimPlay * list [SPRITE_LIST_LEN]);
		virtual void getSounds(Sound * list [SPRITE_LIST_LEN]);
		virtual void update(int delta);
		
	public:
		void takeDamage(int damage);
};
