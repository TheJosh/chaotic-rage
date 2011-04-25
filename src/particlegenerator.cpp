// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

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
	
	for (int i = 0; i < MAX_SPEWERS; i++) {
		this->spewdelay[i] = 0;
	}
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
		if (i > MAX_SPEWERS) break;
		spew = this->type->spewers.at(i);
		
		// If there is a time limit in force, check it hasn't been too long
		if (spew->time != 0 && spew->time < this->age) continue;
		useful = true;
		
		// If there is a delay in force, check enough time has passed
		if (spew->delay) {
			this->spewdelay[i] -= delta;
			if (this->spewdelay[i] > 0) continue;
			this->spewdelay[i] = spew->delay;
		}
		
		// Generate the particles according to the rate
		gennum = ppsDeltai(spew->rate, delta);
		for (int j = 0; j < gennum; j++) {
			pa = new Particle(spew->pt, this->st);
			pa->x = this->x;
			pa->y = this->y;
			
			pa->angle = this->angle + getRandom(0 - spew->angle_range / 2, spew->angle_range / 2);
			pa->angle = clampAngle(pa->angle);
			
			pa->x = pointPlusAngleX(pa->x, pa->angle, spew->offset);
			pa->y = pointPlusAngleY(pa->y, pa->angle, spew->offset);
			
			st->addParticle(pa);
		}
	}
	
	this->age += delta;
	
	if (! useful) {
		this->hasDied();
	}
}


void ParticleGenerator::getSprite(SpritePtr list [SPRITE_LIST_LEN])
{
	list[0] = NULL;
}

void ParticleGenerator::getAnimModel(AnimPlay * list [SPRITE_LIST_LEN])
{
	list[0] = NULL;
}

void ParticleGenerator::getSounds(Sound * list [SPRITE_LIST_LEN])
{
	list[0] = NULL;
}

