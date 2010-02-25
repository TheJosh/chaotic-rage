#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"

using namespace std;


Particle::Particle(ParticleType *pt, GameState *st) : Entity(st)
{
	this->pt = pt;
	
	this->speed = pt->lin_speed + getRandom(-100, 100);
	this->range = pt->range + getRandom(-20, 20);
	
	this->angle = 0;
}

Particle::~Particle()
{
}


void Particle::update(int delta)
{
	this->speed += ppsDelta(this->pt->lin_accel, delta);
	this->range -= ppsDelta(this->speed, delta);
	
	if (this->range <= 0) {
		this->speed -= 500;
	}
	
	if (this->speed <= 0) return;
	
	this->x = pointPlusAngleX(this->x, this->angle, ppsDelta(this->speed, delta));
	this->y = pointPlusAngleY(this->y, this->angle, ppsDelta(this->speed, delta));
}

SDL_Surface* Particle::getSprite()
{
	return this->pt->sprites.at(0);
}


