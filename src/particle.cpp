#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"

using namespace std;


Particle::Particle(ParticleType *pt, GameState *st) : Entity(st)
{
	this->pt = pt;
	this->animation_start = st->anim_frame;
	
	this->speed = pt->lin_speed + getRandom(-100, 100);
	this->range = pt->range + getRandom(-20, 20);
	this->age = getRandom(-600, 100);
	
	this->angle = 0;
}

Particle::~Particle()
{
}


void Particle::update(int delta)
{
	this->age += delta;
	if (this->pt->age != 0 and this->age >= this->pt->age) {
		this->del = true;
	}
	
	this->speed += ppsDelta(this->pt->lin_accel, delta);
	this->range -= ppsDelta(this->speed, delta);
	
	if (this->range <= 0) {
		this->speed -= 500;
	}
	
	if (this->speed <= 0) return;
	
	this->x = pointPlusAngleX(this->x, this->angle, ppsDelta(this->speed, delta));
	this->y = pointPlusAngleY(this->y, this->angle, ppsDelta(this->speed, delta));
	
	this->x += getRandom(-3, 3);
	this->y += getRandom(-3, 3);
}

SDL_Surface* Particle::getSprite()
{
	int idx = 0;
	if (this->pt->directional) idx = round(this->angle / 45);
	
	int frame = this->st->anim_frame - this->animation_start;
	frame = frame % this->pt->num_frames;
	idx += frame;
	
	return this->pt->sprites.at(idx);
}


