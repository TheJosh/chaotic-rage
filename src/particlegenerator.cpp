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
	Entity *e;
	GenSpew *spew;
	int gennum;
	
	bool useful = false;
	for (unsigned int i = 0; i < this->type->spewers.size(); i++) {
		spew = this->type->spewers.at(i);
		
		if (spew->time != 0 && spew->time < this->age) continue;
		
		gennum = ceil(((float)spew->rate) / 1000.0 * delta);
		
		for (int j = 0; j < gennum; j++) {
			e = spew->pt->spawn(this->st);
			e->x = this->x;
			e->y = this->y;
			
			//TODO: needs love
			e->angle = this->angle + getRandom(0 - spew->angle_range / 2, spew->angle_range / 2);
			
			e->x = pointPlusAngleX(e->x, e->angle, spew->offset);
			e->y = pointPlusAngleY(e->y, e->angle, spew->offset);
		}
		
		useful = true;
	}
	
	this->age += delta;
	
	if (! useful) {
		this->del = true;
	}
}


SpritePtr ParticleGenerator::getSprite()
{
	return NULL;
}


