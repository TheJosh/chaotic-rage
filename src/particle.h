// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class Particle: public Entity
{
	public:
		virtual EntityClass klass() const { return PARTICLE; }
		
	public:
		ParticleType* pt;
		
		// copied from particle type
		float speed;
		int accel;
		int max_age;
		int unit_damage;
		int wall_damage;
		int unit_hits;
		int wall_hits;
		
		// general instance variables
		int age;
		
	protected:
		unsigned int animation_start;
		AnimPlay * anim;
		
	public:
		Particle(ParticleType *pt, GameState *st);
		virtual ~Particle();
		
	public:
		virtual void getSprite(SpritePtr list [SPRITE_LIST_LEN]);
		virtual void getAnimModel(AnimPlay * list [SPRITE_LIST_LEN]);
		virtual void update(int delta);
		virtual void hasBeenHit(CollideBox * ours, CollideBox * theirs);
};
