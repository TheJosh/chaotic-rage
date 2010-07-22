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
		int speed;
		int lin_accel;
		int max_age;
		int unit_damage;
		int wall_damage;
		int unit_hits;
		int wall_hits;
		
		// general instance variables
		int age;
		
	protected:
		unsigned int animation_start;
		
	public:
		Particle(ParticleType *pt, GameState *st);
		~Particle();
		
	public:
		virtual void getSprite(SpritePtr list [SPRITE_LIST_LEN]);
		virtual void update(int delta);
		
	public:
		virtual void handleEvent(Event * ev);
};
