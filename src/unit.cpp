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
	
	this->anim = NULL;
	this->current_state_type = 0;
	this->setState(UNIT_STATE_STATIC);
	
	this->walk_state = uc->getState(UNIT_STATE_WALK);
	this->walk_time = 0;
	
	this->remove_at = 0;
}

Unit::~Unit()
{
	delete(this->anim);
}


void Unit::hasBeenHit(CollideBox * ours, CollideBox * theirs)
{
	if (theirs->e->klass() == WALL) {
		this->x = this->old_x;
		this->y = this->old_y;
		this->speed = 0;
		this->setState(UNIT_STATE_STATIC);
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

void Unit::getSounds(Sound * list [SPRITE_LIST_LEN])
{
	if (this->firing && this->weapon != NULL) {
		list[0] = this->weapon->sound_fire;
		list[1] = NULL;
	} else {
		list[0] = NULL;
	}
}


/**
* Moves units around
**/
void Unit::update(int delta, UnitClassSettings *ucs)
{
	if (remove_at != 0) {
		if (remove_at <= st->game_time) this->del = 1;
		return;
	}
	
		int turn_move = ppsDeltai(ucs->turn_move, delta);
	//int turn_aim = ppsDeltai(ucs->turn_aim, delta);
	
	this->desired_angle_move = clampAngle(this->desired_angle_move);
	this->desired_angle_aim = clampAngle(this->desired_angle_aim);
	
	this->angle_move = angleFromDesired(this->angle_move, this->desired_angle_move, turn_move);
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
				|| newx >= this->st->curr_map->width - this->getWidth() || newy >= this->st->curr_map->height - this->getHeight()) {
			this->speed = 0;
			this->setState(UNIT_STATE_STATIC);
			
		} else {
			this->old_x = this->x;
			this->old_y = this->y;
			this->x = newx;
			this->y = newy;
		}
		
		this->walk_time += delta;
	}
	
	this->st->addCollideBox((int) this->x, (int) this->y, 30, this, true);
	
	
	if (this->firing && this->weapon != NULL) {
		if (this->weapon->melee == 0 && this->weapon_gen != NULL) {
			// Bullet-based weapon
			this->weapon_gen->x = (int) round(this->x + this->uc->width / 2);
			this->weapon_gen->y = (int) round(this->y + this->uc->height / 2);
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
void Unit::takeDamage(int damage)
{
	this->health -= damage;
	
	if (this->uc->hit_generator != NULL) {
		ParticleGenerator *pg = new ParticleGenerator(this->uc->hit_generator, st);
		pg->x = this->x;
		pg->y = this->y;
		st->addParticleGenerator(pg);
	}
	
	if (this->health <= 0 && remove_at == 0) {
		Event *ev = new Event();
		ev->type = UNIT_DIED;
		ev->e1 = this;
		fireEvent(ev);
		
		this->speed = 0;
		this->setState(UNIT_STATE_DIE);
		
		remove_at = st->game_time + 10000;
		
		if (this == this->st->curr_player) {
			this->st->curr_player = NULL;
		}
	}
}


