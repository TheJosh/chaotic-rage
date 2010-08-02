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
	
	this->speed = getRandom(pt->lin_speed.min, pt->lin_speed.max);
	this->lin_accel = getRandom(pt->lin_accel.min, pt->lin_accel.max);
	this->max_age = getRandom(pt->age.min, pt->age.max);
	this->unit_damage = getRandom(pt->unit_damage.min, pt->unit_damage.max);
	this->wall_damage = getRandom(pt->wall_damage.min, pt->wall_damage.max);
	this->unit_hits = getRandom(pt->unit_hits.min, pt->unit_hits.max);
	this->wall_hits = getRandom(pt->wall_hits.min, pt->wall_hits.max);
	
	this->angle = 0;
	this->age = 0;
}

Particle::~Particle()
{
}

/**
* Handle events
**/
void Particle::handleEvent(Event * ev)
{
	this->pt->doActions(this, ev->type);
}


void Particle::update(int delta)
{
	this->age += delta;
	if (this->age >= this->max_age) {
		this->del = true;
	}
	
	this->speed += ppsDelta(this->lin_accel, delta);
	
	if (this->speed <= 0) return;
	
	this->x = pointPlusAngleX(this->x, this->angle, ppsDelta(this->speed, delta));
	this->y = pointPlusAngleY(this->y, this->angle, ppsDelta(this->speed, delta));
	
	this->x += getRandom(-3, 3);
	this->y += getRandom(-3, 3);
	
	
	// Hit a wall?
	data_pixel pixel = this->st->map->getDataAt(this->x, this->y);
	
	AreaType *at = pixel.type;
	if (at->wall) {
		Event *ev = new Event();
		ev->type = PART_HIT_WALL;
		ev->e1 = this;
		fireEvent(ev);
		
		this->st->map->setDataHP(this->x, this->y, pixel.hp - this->wall_damage);
		
		this->wall_hits--;
		if (this->wall_hits == 0) this->del = true;
	}
	
}

void Particle::getSprite(SpritePtr list [SPRITE_LIST_LEN])
{
	int idx = 0;
	if (this->pt->directional) idx = round(this->angle / 45);
	
	int frame = this->st->anim_frame - this->animation_start;
	frame = frame % this->pt->num_frames;
	idx += frame;
	
	list[0] = this->pt->sprites.at(idx);
	
	return;
}


