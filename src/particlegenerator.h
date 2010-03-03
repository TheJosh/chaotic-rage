#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class ParticleGenerator : public Entity {
	public:
		ParticleGenType* type;
		int x;
		int y;
		int angle;
		int age;
		
	public:
		ParticleGenerator(ParticleGenType* type, GameState *st);
		~ParticleGenerator();
		
	public:
		virtual void update(int delta);
		virtual SDL_Surface* getSprite();
};
