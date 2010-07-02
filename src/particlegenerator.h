#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class ParticleGenerator : public Entity
{
	public:
		virtual EntityClass klass() const { return PGENERATOR; }
		
	public:
		ParticleGenType* type;
		int x;
		int y;
		int age;
		
	public:
		ParticleGenerator(ParticleGenType* type, GameState *st);
		~ParticleGenerator();
		
	public:
		virtual void update(int delta);
		virtual SpritePtr getSprite();
};
