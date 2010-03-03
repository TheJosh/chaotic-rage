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
	this->angle = 0;
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
	
	bool useful = false;
	for (unsigned int i = 0; i < this->type->spewers.size(); i++) {
		spew = this->type->spewers.at(i);
		
		if (spew->time != 0 && spew->time < this->age) continue;
		
		gennum = ceil(((float)spew->rate) / 1000.0 * delta);
		
		for (int j = 0; j < gennum; j++) {
			pa = new Particle(spew->pt, this->st);
			pa->x = this->x;
			pa->y = this->y;
			
			//TODO: needs love
			pa->angle = this->angle + getRandom(0 - spew->angle_range / 2, spew->angle_range / 2);
			
			st->addParticle(pa);
		}
		
		useful = true;
	}
	
	this->age += delta;
	
	if (! useful) {
		this->del = true;
	}
}


SDL_Surface* ParticleGenerator::getSprite()
{
	return NULL;
}


