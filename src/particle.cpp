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
	
	this->angle = 0;
	this->age = 0;
	this->z = 40;		// in the air a little
	
	this->radius = 2;
	if (this->unit_damage + this->wall_damage == 0) this->collide = false;
}

Particle::~Particle()
{
	delete (this->anim);
}

/**
* Handle events
**/
void Particle::handleEvent(Event * ev)
{
	this->pt->doActions(ev);
	
	if (ev->type == ENTITY_HIT) {
		Entity *e = (ev->e1 == this ? ev->e2 : ev->e1);
		
		if (e->klass() == UNIT) {
			// We hit a unit
			if (this->unit_damage > 0) {
				((Unit*)e)->takeDamage(this->unit_damage);
				
				this->unit_hits--;
				if (this->unit_hits == 0) this->hasDied();
			
			} else {
				this->speed = 0;
			}
		
		} else if (e->klass() == WALL) {
			// We hit a wall
			if (this->wall_damage > 0) {
				((Wall*)e)->takeDamage(this->wall_damage);
				
				this->wall_hits--;
				if (this->wall_hits == 0) this->hasDied();
			
			} else {
				this->speed = 0;
			}
			
		}
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
	
	this->speed += ppsDelta(this->accel, delta);
	
	this->x = pointPlusAngleX(this->x, this->angle, ppsDelta(this->speed, delta));
	this->y = pointPlusAngleY(this->y, this->angle, ppsDelta(this->speed, delta));
	
	this->x += getRandom(-3, 3);
	this->y += getRandom(-3, 3);
	
	if (this->unit_damage + this->wall_damage == 0) return;
	
	
	if (this->anim->isDone()) this->anim->next();
}

void Particle::getSprite(SpritePtr list [SPRITE_LIST_LEN])
{
	int idx = 0;
	if (this->pt->directional) idx = (int) round(this->angle / 45);
	
	int frame = this->st->anim_frame - this->animation_start;
	frame = frame % this->pt->num_frames;
	idx += frame;
	
	list[0] = this->pt->sprites.at(idx);
}

void Particle::getAnimModel(AnimPlay * list [SPRITE_LIST_LEN])
{
	list[0] = this->anim;
	list[1] = NULL;
}

