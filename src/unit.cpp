// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"

using namespace std;


Unit::Unit(UnitClass *uc, GameState *st) : Entity(st)
{
	this->uc = uc;
	this->angle_move = 0;
	this->desired_angle_move = 0;
	this->angle_aim = 0;
	this->desired_angle_aim = 0;
	this->speed = 0;
	this->health = 10000;
	this->height = this->uc->height;
	
	this->weapon = NULL;
	this->weapon_gen = NULL;
	this->firing = false;
	
	this->pickupWeapon(getWeaponTypeByID(0));
	
	this->current_state_type = 0;
	this->setState(UNIT_STATE_STATIC);
	
	this->sprites = uc->loadAllSprites();
}

Unit::~Unit()
{
	unsigned int j;
	for (j = 0; j < this->sprites->size(); j++) {
		st->render->freeSprite(this->sprites->at(j));
	}
	
	delete(this->sprites);
}


/**
* Sets the unit state. If the unit state type changes, a new state is randomly
* chosen with the specified state type
**/
void Unit::setState(int new_type)
{
	if (this->current_state_type == new_type) return;
	
	if (new_type == UNIT_STATE_STATIC && this->firing) new_type = UNIT_STATE_FIRING;
	
	this->current_state_type = new_type;
	this->current_state = this->uc->getState(new_type);
	this->animation_start = this->st->anim_frame;
}


/**
* Sets the unit weapon
**/
void Unit::setWeapon(WeaponType* wt)
{
	this->weapon = wt;
	this->firing = false;
	
	delete this->weapon_gen;
	this->weapon_gen = NULL;
	
	if (wt->pg) {
		this->weapon_gen = new ParticleGenerator(wt->pg, this->st);
	}
}


void Unit::beginFiring()
{
	if (this->weapon == NULL) return;
	
	this->firing = true;
	
	if (this->weapon_gen) {
		this->weapon_gen->age = 0;
	}
	
	if (this->speed == 0) {
		this->setState(UNIT_STATE_FIRING);
	}
}

void Unit::endFiring()
{
	if (this->weapon == NULL) return;
	
	this->firing = false;
	
	if (this->speed == 0) {
		this->setState(UNIT_STATE_STATIC);
	}
}

void Unit::pickupWeapon(WeaponType* wt)
{
	this->avail_weapons.push_back(wt);
	
	if (this->weapon == NULL) {
		this->setWeapon(wt);
	}
}

unsigned int Unit::getNumWeapons()
{
	return this->avail_weapons.size();
}

WeaponType* Unit::getWeaponByID(unsigned int id)
{
	return this->avail_weapons.at(id);
}


/**
* Determines the unit sprite
**/
SpritePtr Unit::getSprite()
{
	int idx = round(this->angle_aim / 45);
	
	int frame = this->st->anim_frame - this->animation_start;
	frame = frame % this->current_state->num_frames;
	
	idx *= this->uc->getMaxFrames();
	idx += frame;
	
	idx += this->current_state->sprite_offset;
	
	return this->sprites->at(idx);
}


/**
* Moves units around
**/
void Unit::update(int delta, UnitClassSettings *ucs)
{
	int turn_move = ppsDelta(ucs->turn_move, delta);
	int turn_aim = ppsDelta(ucs->turn_aim, delta);
	
	this->angle_move = angleFromDesired(this->angle_move, this->desired_angle_move, turn_move);
	this->angle_aim = angleFromDesired(this->angle_aim, this->desired_angle_aim, turn_aim);
	
	// Movement
	if (this->speed != 0) {
		int newx = pointPlusAngleX(this->x, this->angle_move, ppsDelta(this->speed, delta));
		int newy = pointPlusAngleY(this->y, this->angle_move, ppsDelta(this->speed, delta));
	
		// Collision detection
		if (collideWall(this->st, newx, newy, this->uc->width, this->uc->height)) {
			this->speed = 0;
			this->setState(UNIT_STATE_STATIC);
		
		} else {
			this->x = newx;
			this->y = newy;
		}
	}
	
	// Have we been hit?
	vector<Particle*> * particles = this->st->particlesInside(this->x, this->y, this->uc->width, this->uc->height);
	
	unsigned int i;
	for (i = 0; i < particles->size(); i++) {
		Particle *p = particles->at(i);
		
		p->pt->doActions(p, HIT_UNIT);
		this->uc->doActions(this, HIT_PARTICLE);
		
		this->health -= p->unit_damage;
		if (this->health <= 0) {
			this->del = true;
		}
		
		p->unit_hits--;
		if (p->unit_hits == 0) p->del = true;
	}
	
	delete (particles);
	
	// Fire bullets
	if (this->firing and this->weapon_gen) {
		this->weapon_gen->x = this->x + this->uc->width / 2;
		this->weapon_gen->y = this->y + this->uc->height / 2;
		this->weapon_gen->angle = this->angle_aim;
		
		this->weapon_gen->update(delta);
	}
}


