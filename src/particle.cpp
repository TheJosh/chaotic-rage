// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"

using namespace std;


Particle::Particle(ParticleType *pt, GameState *st) : Entity(st)
{
	this->pt = pt;
	this->animation_start = st->anim_frame;
	
	this->speed = getRandom(pt->begin_speed.min, pt->begin_speed.max);
	this->accel = getRandom(pt->accel.min, pt->accel.max);
	this->max_age = getRandom(pt->age.min, pt->age.max);
	this->unit_damage = getRandom(pt->unit_damage.min, pt->unit_damage.max);
	this->wall_damage = getRandom(pt->wall_damage.min, pt->wall_damage.max);
	this->unit_hits = getRandom(pt->unit_hits.min, pt->unit_hits.max);
	this->wall_hits = getRandom(pt->wall_hits.min, pt->wall_hits.max);
	
	this->anim = new AnimPlay(pt->model);
	this->cb = NULL;
	
	this->angle = 0;
	this->age = 0;
	this->z = 40;		// in the air a little
}

Particle::~Particle()
{
	delete (this->anim);
	this->st->delCollideBox(this->cb);
}


void Particle::doHitUnit(Unit *u)
{
	if (this->unit_damage > 0) {
		u->takeDamage(this->unit_damage);
		
		this->unit_hits--;
		if (this->unit_hits == 0) {
			this->hasDied();
		}
		
	} else {
		this->speed = 0;
	}
}

void Particle::doHitWall(Wall *w)
{
	if (this->wall_damage > 0) {
		w->takeDamage(this->wall_damage);
		
		this->wall_hits--;
		if (this->wall_hits == 0) {
			this->hasDied();
		}
	
	} else {
		this->speed = 0;
	}
}


void Particle::update(int delta)
{
	this->age += delta;
	if (this->age >= this->max_age) {
		this->hasDied();
	}
	
	if (this->speed <= 0) {
		this->z = 0;
		return;
	}
	
	this->speed += ppsDeltaf(this->accel, delta);
	
	this->x = pointPlusAngleX(this->x, this->angle, ppsDeltaf(this->speed, delta));
	this->y = pointPlusAngleY(this->y, this->angle, ppsDeltaf(this->speed, delta));
	
	//this->x += getRandom(-3, 3);
	//this->y += getRandom(-3, 3);
	
	// TODO: check still in bounds, if not, remove
	
	
	if (this->cb == NULL) {
		this->cb = this->st->addCollideBox(0, 0, 2, this, false);
	} else {
		this->st->moveCollideBox(this->cb, (int) this->x, (int) this->y);
	}
	
	
	if (this->anim->isDone()) this->anim->next();
}

AnimPlay* Particle::getAnimModel()
{
	return this->anim;
}

Sound* Particle::getSound()
{
	return NULL;
}

