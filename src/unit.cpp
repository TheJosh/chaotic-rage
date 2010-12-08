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
	this->health = uc->begin_health;
	
	this->weapon = NULL;
	this->weapon_gen = NULL;
	this->firing = false;
	
	this->pickupWeapon(this->uc->getMod()->getWeaponType(0));
	
	this->current_state_type = 0;
	this->setState(UNIT_STATE_STATIC);
	
	this->walk_state = uc->getState(UNIT_STATE_WALK);
	this->walk_time = 0;
	
	
	this->anim = new AnimPlay(this->walk_state->model);
	
}

Unit::~Unit()
{
	delete(this->anim);
}


/**
* Handle events
**/
void Unit::handleEvent(Event * ev)
{
	this->uc->doActions(ev);
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

/**
* Pick up a weapon
**/
void Unit::pickupWeapon(WeaponType* wt)
{
	this->avail_weapons.push_back(wt);
	
	if (this->weapon == NULL) {
		this->setWeapon(0);
	}
}

unsigned int Unit::getNumWeapons()
{
	return this->avail_weapons.size();
}

WeaponType * Unit::getWeaponAt(unsigned int id)
{
	return this->avail_weapons[id];
}

/**
* Sets the unit weapon
* id is the weapon id of the units personal weapon cache - not the weapontype id
**/
void Unit::setWeapon(int id)
{
	WeaponType *wt = this->avail_weapons.at(id);
	
	this->weapon = wt;
	this->firing = false;
	
	delete this->weapon_gen;
	this->weapon_gen = NULL;
	
	if (wt->pg) {
		this->weapon_gen = new ParticleGenerator(wt->pg, this->st);
	}
	
	curr_weapon_id = id;
}

unsigned int Unit::getCurrentWeaponID()
{
	return this->curr_weapon_id;
}

unsigned int Unit::getPrevWeaponID()
{
	int ret = this->curr_weapon_id - 1;
	if (ret < 0) ret = this->avail_weapons.size() - 1;
	return ret;
}

unsigned int Unit::getNextWeaponID()
{
	unsigned int ret = this->curr_weapon_id + 1;
	if (ret > this->avail_weapons.size() - 1) ret = 0;
	return ret;
}


/**
* Determines the unit sprite
**/
void Unit::getSprite(SpritePtr list [SPRITE_LIST_LEN])
{
	list[0] = NULL;
	return;
}

void Unit::getAnimModel(AnimPlay * list [SPRITE_LIST_LEN])
{
	list[0] = this->anim;
	list[1] = NULL;
}


/**
* Moves units around
**/
void Unit::update(int delta, UnitClassSettings *ucs)
{
	int turn_move = ppsDelta(ucs->turn_move, delta);
	int turn_aim = ppsDelta(ucs->turn_aim, delta);
	
	this->desired_angle_move = clampAngle(this->desired_angle_move);
	this->desired_angle_aim = clampAngle(this->desired_angle_aim);
	
	this->angle_move = angleFromDesired(this->angle_move, this->desired_angle_move, turn_move);
	this->angle_aim = angleFromDesired(this->angle_aim, this->desired_angle_aim, turn_aim);
	
	this->angle = this->angle_aim;
	
	
	if (this->speed > ucs->max_speed) this->speed = ucs->max_speed;
	if (this->speed < 0 - ucs->max_speed) this->speed = 0 - ucs->max_speed;
	
	// Movement
	if (this->speed != 0) {
		int newx = pointPlusAngleX(this->x, this->angle_move, ppsDelta(this->speed, delta));
		int newy = pointPlusAngleY(this->y, this->angle_move, ppsDelta(this->speed, delta));
		
		
		// Collision detection
		if (newx < 0 || newy < 0
				|| newx >= this->st->map->width - this->getWidth() || newy >= this->st->map->height - this->getHeight()) {
			this->speed = 0;
			this->setState(UNIT_STATE_STATIC);
			
		} else if (this->st->checkHitWall(newx, newy, this->uc->width) != NULL) {
			this->speed = 0;
			this->setState(UNIT_STATE_STATIC);
			
		} else {
			this->x = newx;
			this->y = newy;
		}
		
		this->walk_time += delta;
	}
	
	// Fire bullets
	if (this->firing and this->weapon_gen) {
		this->weapon_gen->x = this->x + this->uc->width / 2;
		this->weapon_gen->y = this->y + this->uc->height / 2;
		this->weapon_gen->angle = this->angle_aim;
		
		this->weapon_gen->update(delta);
	}
	
	if (this->anim->isDone()) this->anim->next();
}


/**
* We have been hit! Take some damage
**/
void Unit::takeDamage(int damage)
{
	this->health -= damage;
	if (this->health <= 0) {
		this->del = 1;
	}
}


