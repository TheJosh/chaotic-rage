#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include "rage.h"

using namespace std;


ParticleGenerator::ParticleGenerator(ParticleGenType* type, GameState *st) : Entity(st)
{
	this->type = type;
	this->x = x;
	this->y = y;
	this->age = 0;
}

ParticleGenerator::~ParticleGenerator()
{
}


/**
* Creates particles
**/
void ParticleGenerator::update(int delta)
{
	Particle *pa;
	GenSpew *spew;
	int gennum;
	
	for (unsigned int i = 0; i < this->type->spewers.size(); i++) {
		spew = this->type->spewers.at(i);
		
		if (spew->time < this->age) continue;
		
		gennum = ceil(((float)spew->qty) / ((float)spew->time) * delta);
		
		for (int j = 0; j < gennum; j++) {
			pa = new Particle(spew->pt, this->st);
			pa->x = this->x;
			pa->y = this->y;
			pa->angle = getRandom(0, spew->angle_range);
			
			st->addParticle(pa);
		}
	}
	
	this->age += delta;
}


SDL_Surface* ParticleGenerator::getSprite()
{
	return NULL;
}


