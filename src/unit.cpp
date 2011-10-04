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
	
	this->curr_obj = NULL;
	this->lift_obj = NULL;
	this->drive_obj = NULL;
	this->turret_obj = NULL;
	
	this->closest = NULL;
	this->melee_time = 0;
	this->melee_cooldown = 0;
	
	this->weapon_sound = -1;
	
	
	// access sounds using this->uc->getSound(type)
	// see unittype.h for types (e.g. UNIT_SOUND_DEATH)
	// example:
	// Sound* snd;if (this->firing
	// snd = this->uc->getSound(UNIT_SOUND_DEATH);
	// if (snd != NULL) this->st->audio->playSound(snd);
}

Unit::~Unit()
{
	delete(this->anim);
	this->st->delCollideBox(this->cb);
}


/**
* Hit!
**/
void Unit::hasBeenHit(CollideBox * ours, CollideBox * theirs)
{
	if (remove_at != 0) return;
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
* Near miss!
**/
void Unit::entityClose(Entity * e, float dist)
{
	if (e->klass() == UNIT) {
		closest = (Unit*) e;
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
	
	Sound* snd = this->weapon->getSound(WEAPON_SOUND_BEGIN);
	weapon_sound = this->st->audio->playSound(snd, true, this);
}

void Unit::endFiring()
{
	if (this->weapon == NULL) return;
	
	this->firing = false;
	
	if (this->speed == 0) {
		this->setState(UNIT_STATE_STATIC);
	}
	
	this->st->audio->stopSound(this->weapon_sound);

	Sound* snd = this->weapon->getSound(WEAPON_SOUND_END);
	this->st->audio->playSound(snd, false, this);
}

void Unit::meleeAttack()
{
	if (this->melee_time != 0) return;
	if (this->melee_cooldown != 0) return;

	this->melee_time = st->game_time + this->uc->melee_delay;
	this->melee_cooldown = this->melee_time + this->uc->melee_cooldown;

	this->setState(UNIT_STATE_MELEE);
}

void Unit::specialAttack()
{
	// TODO: Code this one
	this->st->hud->addAlertMessage("TODO: special attack");
}


/**
* Pick up a weapon
**/
int Unit::pickupWeapon(WeaponType* wt)
{
	this->avail_weapons.push_back(wt);
	
	if (this->weapon == NULL) {
		this->setWeapon(0);
	}

	return this->avail_weapons.size() - 1;
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
	
	
	if (st->server) st->server->addmsgUnitUpdate(this);
	
	
	// Do some shooting
	WeaponType *w = NULL;
	if (this->firing) {
		if (this->turret_obj) {
			w = this->st->getDefaultMod()->getWeaponType("poopgun");
			
		} else if (this->drive_obj) {
			w = this->st->getDefaultMod()->getWeaponType("rocket_launcher");
			
		} else if (this->weapon) {
			w = this->weapon;
		}
	}
	
	// I still might remove weapon effects using a particle generator
	// It's a dumb system anyway.
	/*if (this->weapon_gen != NULL) {
		this->weapon_gen->x = this->x;
		this->weapon_gen->y = this->y;
		this->weapon_gen->angle = this->angle_aim;
		this->weapon_gen->update(delta);
	}*/
	
	if (w && w->pt) {
		Particle* pa = new Particle(w->pt, this->st);
		pa->x = this->x;
		pa->y = this->y;
		
		pa->angle = this->angle + getRandom(0 - w->angle_range / 2, w->angle_range / 2);
		pa->angle = clampAngle(pa->angle);
		
		// TODO: This should be dynamic or computed or something even better (vectors anyone?)
		pa->x = pointPlusAngleX(pa->x, pa->angle, 50);
		pa->y = pointPlusAngleY(pa->y, pa->angle, 50);
		
		st->addParticle(pa);
		
		if (! w->continuous) this->firing = false;
	}
	
	// Melee
	if (this->melee_time != 0 && this->melee_time < st->game_time) {
		if (this->closest) this->closest->takeDamage(this->uc->melee_damage);
		this->melee_time = 0;

	} else if (this->melee_cooldown != 0 && this->melee_cooldown < st->game_time) {
		this->setState(UNIT_STATE_STATIC);
		this->melee_cooldown = 0;
	}


	// Move the lifted object with the unit
	// Or driving, or turrets
	if (this->lift_obj) {
		this->lift_obj->x = this->x;
		this->lift_obj->y = this->y;
		this->lift_obj->z = 70;
		this->lift_obj->angle = this->angle;
		
	} else if (this->drive_obj) {
		this->drive_obj->x = pointPlusAngleX(this->x, this->angle, -20);
		this->drive_obj->y = pointPlusAngleY(this->y, this->angle, -20);
		this->drive_obj->z = this->z;
		this->drive_obj->angle = this->angle;
		
	} else if (this->turret_obj) {
		this->x = this->turret_obj->x;
		this->y = this->turret_obj->y;
		this->z = this->turret_obj->z + 20;
		this->turret_obj->angle = this->angle;
	}
	
	
	// This will be re-set by the collission code
	this->curr_obj = NULL;
	this->closest = NULL;
	
	
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
		
		remove_at = st->game_time + (10 * 60 * 1000);	// 10 mins
		
		return 1;
	}
	
	return 0;
}


/**
* Use an object
**/
void Unit::doUse()
{
	if (this->curr_obj == NULL) return;
	
	
	ObjectType *ot = this->curr_obj->ot;

	if (ot->show_message.length() != 0) {
		this->st->hud->addAlertMessage(ot->show_message);
	}

	if (ot->add_object.length() != 0) {
		Object *nu = new Object(this->st->getDefaultMod()->getObjectType(ot->add_object), this->st);
		nu->x = this->x;
		nu->y = this->y;
		nu->z = 60;
		this->st->addObject(nu);
	}

	if (ot->pickup_weapon.length() != 0) {
		WeaponType *wt = this->st->getDefaultMod()->getWeaponType(ot->pickup_weapon);
		if (wt) {
			this->st->hud->addAlertMessage("Picked up a ", wt->title);
			this->setWeapon(this->pickupWeapon(wt));
		}
	}

	if (ot->drive == 1) {
		if (this->drive_obj) {
			this->drive_obj = NULL;
		} else {
			this->drive_obj = this->curr_obj;
		}

	} else if (ot->turret == 1) {
		if (this->turret_obj) {
			this->turret_obj = NULL;
			this->setWeapon(this->curr_weapon_id);
		} else {
			this->turret_obj = this->curr_obj;
			this->weapon_gen = new ParticleGenerator(st->getDefaultMod()->getWeaponType("poopgun")->pg, this->st);
		}
	}
}

/**
* Lift an object
**/
void Unit::doLift()
{
	this->lift_obj = this->curr_obj;
}

/**
* Drop an object. Is this the same as throw?
**/
void Unit::doDrop()
{
	this->lift_obj->speed = 10;

	this->lift_obj = NULL;
}
