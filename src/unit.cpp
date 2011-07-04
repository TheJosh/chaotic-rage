// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"

using namespace std;


Unit::Unit(UnitType *uc, GameState *st) : Entity(st)
{
	this->uc = uc;
	this->angle_move = 0;
	this->desired_angle_move = 0;
	this->angle_aim = 0;
	this->desired_angle_aim = 0;
	this->speed = 0;
	this->health = uc->begin_health;
	this->slot = 0;
	
	this->weapon = NULL;
	this->weapon_gen = NULL;
	this->firing = false;
	
	this->anim = NULL;
	this->current_state_type = 0;
	this->setState(UNIT_STATE_STATIC);
	
	this->cb = NULL;
	
	this->walk_state = uc->getState(UNIT_STATE_WALK);
	this->walk_time = 0;
	
	this->remove_at = 0;
	
	
	// access sounds using this->uc->getSound(type)
	// see unittype.h for types (e.g. UNIT_SOUND_DEATH)
	// example:
	// Sound* snd;
	// snd = this->uc->getSound(UNIT_SOUND_DEATH);
	// if (snd != NULL) this->st->audio->playSound(snd);
}

Unit::~Unit()
{
	delete(this->anim);
	this->st->delCollideBox(this->cb);
}


void Unit::hasBeenHit(CollideBox * ours, CollideBox * theirs)
{
	if (theirs->e->klass() == WALL) {
		this->x = this->old_x;
		this->y = this->old_y;
		this->speed = 0;
		this->setState(UNIT_STATE_STATIC);
		
	} else if (theirs->e->klass() == PARTICLE) {
		((Particle*)theirs->e)->doHitUnit(this);
	}
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
	
	delete (this->anim);
	this->anim = new AnimPlay(this->current_state->model);
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


AnimPlay* Unit::getAnimModel()
{
	return this->anim;
}

Sound* Unit::getSound()
{
	if (this->firing && this->weapon != NULL) {
		return this->weapon->sound_fire;
	}
	
	return NULL;
}



/**
* Moves units around
**/
void Unit::update(int delta, UnitTypeSettings *ucs)
{
	if (remove_at != 0) {
		if (remove_at <= st->game_time) this->del = 1;
		return;
	}
	
	int turn_move = ppsDeltai(ucs->turn_move, delta);
	
	this->desired_angle_move = clampAngle(this->desired_angle_move);
	this->angle_move = angleFromDesired(this->angle_move, this->desired_angle_move, turn_move);
	
	//this->desired_angle_aim = clampAngle(this->desired_angle_aim);
	//int turn_aim = ppsDeltai(ucs->turn_aim, delta);
	//this->angle_aim = angleFromDesired(this->angle_aim, this->desired_angle_aim, turn_aim);
	
	this->angle = this->angle_aim;
	
	if (this->speed > ucs->max_speed) this->speed = ucs->max_speed;
	if (this->speed < 0 - ucs->max_speed) this->speed = 0 - ucs->max_speed;
	
	
	// Movement
	if (this->speed != 0.0) {
		float newx = pointPlusAngleX(this->x, this->angle_move, ppsDeltaf(this->speed, delta));
		float newy = pointPlusAngleY(this->y, this->angle_move, ppsDeltaf(this->speed, delta));
		
		// Fall off the map detection
		if (newx < 0.0 || newy < 0.0
				|| newx >= this->st->curr_map->width - this->uc->width || newy >= this->st->curr_map->height - this->uc->height) {
			this->speed = 0;
			this->setState(UNIT_STATE_STATIC);
			
		} else {
			this->old_x = this->x;
			this->old_y = this->y;
			this->x = newx;
			this->y = newy;
		}
		
		this->walk_time += delta;
		
		
		if (this->cb == NULL) {
			this->cb = this->st->addCollideBox(0, 0, 30, this, true);
		} else {
			this->st->moveCollideBox(this->cb, (int) this->x, (int) this->y);
		}
	}
	
	
	if (this->firing && this->weapon != NULL) {
		if (this->weapon->melee == 0 && this->weapon_gen != NULL) {
			// Bullet-based weapon
			this->weapon_gen->x = this->x;
			this->weapon_gen->y = this->y;
			this->weapon_gen->angle = this->angle_aim;
			this->weapon_gen->update(delta);
			
		} else if (this->weapon->melee == 1) {
			// Meele-based weapon
			/*Unit * target = (Unit*) this->inContactWith(UNIT);
			if (target != NULL) {
				target->takeDamage(this->weapon->damage);
			}*/
		}
	}
	
	
	if (this->anim->isDone()) this->anim->next();
}


/**
* We have been hit! Take some damage
**/
int Unit::takeDamage(int damage)
{
	this->health -= damage;
	
	if (this->health <= 0 && remove_at == 0) {
		this->speed = 0;
		this->setState(UNIT_STATE_DIE);
		
		remove_at = st->game_time + 10000;
		
		return 1;
	}
	
	return 0;
}


