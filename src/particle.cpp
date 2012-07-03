// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"

using namespace std;


Particle::Particle(ParticleType *pt, GameState *st, float x, float y, float z) : Entity(st)
{
	this->pt = pt;
	this->animation_start = st->anim_frame;
	
	this->accel = getRandom(pt->accel.min, pt->accel.max);
	this->max_age = getRandom(pt->age.min, pt->age.max);
	this->unit_damage = getRandom(pt->unit_damage.min, pt->unit_damage.max);
	this->wall_damage = getRandom(pt->wall_damage.min, pt->wall_damage.max);
	this->unit_hits = getRandom(pt->unit_hits.min, pt->unit_hits.max);
	this->wall_hits = getRandom(pt->wall_hits.min, pt->wall_hits.max);
	
	this->anim = new AnimPlay(pt->model);
	this->cb = NULL;
	this->age = 0;
	
	
	// TODO: The colShape should be tied to the wall type.
	btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
	
	// TODO: Store the colshape and nuke at some point
	// collisionShapes.push_back(colShape);
	
	this->body = st->physics->addRigidBody(colShape, 0.1, x, y, z);
	
	this->body->setUserPointer(this);
}

Particle::~Particle()
{
	delete (this->anim);
	st->physics->delRigidBody(this->body);
}


void Particle::doHitUnit(Unit *u)
{return;
	if (this->unit_damage > 0) {
		u->takeDamage(this->unit_damage);
		
		this->unit_hits--;
		if (this->unit_hits == 0) {
			DEBUG("part", "%p dead, unit-hit", this);
			this->hasDied();
		}
	}
}

void Particle::doHitWall(Wall *w)
{return;
	if (this->wall_damage > 0) {
		w->takeDamage(this->wall_damage);
		
		this->wall_hits--;
		if (this->wall_hits == 0) {
			DEBUG("part", "%p dead, wall-hit", this);
			this->hasDied();
		}
	}
}


void Particle::update(int delta)
{
	this->age += delta;
	if (this->age >= this->max_age) {
		DEBUG("part", "%p dead, age", this);
		this->hasDied();
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

