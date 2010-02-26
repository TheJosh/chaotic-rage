#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class Particle: public Entity
{
	public:
		int speed;
		int angle;
		int range;
		
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
