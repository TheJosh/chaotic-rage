#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"

using namespace std;


Particle::Particle(ParticleType *pt, GameState *st) : Entity(st)
{
	this->pt = pt;
	this->animation_start = st->anim_frame;
	
	this->speed = getRandom(pt->lin_speed.min, pt->lin_speed.max);
	this->lin_accel = getRandom(pt->lin_accel.min, pt->lin_accel.max);
	this->max_age = getRandom(pt->age.min, pt->age.max);
	this->damage = getRandom(pt->damage.min, pt->damage.max);
	this->damage_accel = getRandom(pt->damage_accel.min, pt->damage_accel.max);
	
	this->angle = 0;
	this->age = 0;
}

Particle::~Particle()
{
}


void Particle::update(int delta)
{
	this->age += delta;
	if (this->age >= this->max_age) {
		this->del = true;
	}
	
	this->speed += ppsDelta(this->lin_accel, delta);
	this->damage += ppsDelta(this->damage_accel, delta);
	
	if (this->speed <= 0) return;
	
	this->x = pointPlusAngleX(this->x, this->angle, ppsDelta(this->speed, delta));
	this->y = pointPlusAngleY(this->y, this->angle, ppsDelta(this->speed, delta));
	
	this->x += getRandom(-3, 3);
	this->y += getRandom(-3, 3);
	
	
	AreaType *at = getAreaTypeByID(this->st->map->getDataAt(this->x, this->y));
	if (at->wall) {
		this->del = true;
		
		this->pt->doActions(this, HIT_WALL);
	}
	
}

SpritePtr Particle::getSprite()
{
	int idx = 0;
	if (this->pt->directional) idx = round(this->angle / 45);
	
	int frame = this->st->anim_frame - this->animation_start;
	frame = frame % this->pt->num_frames;
	idx += frame;
	
	return this->pt->sprites.at(idx);
}


