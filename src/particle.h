#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class Particle: public Entity
{
	public:
		// copied from particle type
		int speed;
		int lin_accel;
		int max_age;
		
		// general instance variables
		int angle;
		int age;
		
	protected:
		ParticleType* pt;
		unsigned int animation_start;
		
	public:
		Particle(ParticleType *pt, GameState *st);
		~Particle();
		
	public:
		virtual SDL_Surface* getSprite();
		virtual void update(int delta);
};
