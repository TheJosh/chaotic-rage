// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include <btBulletDynamicsCommon.h>
#include "rage.h"

using namespace std;


Unit::Unit(UnitType *uc, GameState *st, float x, float y, float z) : Entity(st)
{
	this->uc = uc;
	this->health = uc->begin_health;
	this->slot = 0;
	
	this->weapon = NULL;
	this->firing = false;
	
	this->anim = NULL;
	this->current_state_type = 0;
	this->setState(UNIT_STATE_STATIC);
	
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
	
	
	// TODO: The colShape should be tied to the wall type.
	btCollisionShape* colShape = new btBoxShape(btVector3(1.f, 1.f, 2.f));
	
	// TODO: Store the colshape and nuke at some point
	// collisionShapes.push_back(colShape);
	
	this->body = st->physics->addRigidBody(colShape, 0.1, x, y, z);
	
	this->body->setUserPointer(this);
	
	
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
	st->physics->delRigidBody(this->body);
}


/**
* Hit!
**/
void Unit::hasBeenHit(Entity * that)
{
	DEBUG("coll", "hasBeenHit %p %p", this, that);
	
	if (remove_at != 0) return;
	if (that->klass() == WALL) {
		this->setState(UNIT_STATE_STATIC);
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
	
	this->setState(UNIT_STATE_FIRING);
	
	Sound* snd = this->weapon->wt->getSound(WEAPON_SOUND_BEGIN);
	weapon_sound = this->st->audio->playSound(snd, true, this);
}

void Unit::endFiring()
{
	if (this->weapon == NULL) return;
	
	this->firing = false;
	
	this->setState(UNIT_STATE_STATIC);
	
	this->st->audio->stopSound(this->weapon_sound);

	Sound* snd = this->weapon->wt->getSound(WEAPON_SOUND_END);
	this->st->audio->playSound(snd, false, this);
}


/**
* What is directly in front of a unit, upto range meters ahead
**/
Entity * Unit::infront(float range)
{
	btTransform xform;
	body->getMotionState()->getWorldTransform(xform);
	btVector3 forwardDir = xform.getBasis()[1];
	forwardDir.normalize();
	
	// Begin and end vectors
	btVector3 begin = xform.getOrigin();
	btVector3 end = begin + forwardDir * btScalar(0.0f - range);
	
	// Do the rayTest
	btCollisionWorld::ClosestRayResultCallback cb(begin, end);
	this->st->physics->getWorld()->rayTest(begin, end, cb);
	
	// Check the raytest result
	if (cb.hasHit()) {
		btRigidBody * body = btRigidBody::upcast(cb.m_collisionObject);
		if (body) {
			return static_cast<Entity*>(body->getUserPointer());
		}
	}
	
	return NULL;
}


void Unit::meleeAttack()
{
	if (this->melee_time != 0) return;
	if (this->melee_cooldown != 0) return;
	
	this->melee_time = st->game_time + this->uc->melee_delay;
	this->melee_cooldown = this->melee_time + this->uc->melee_cooldown;
	
	this->setState(UNIT_STATE_MELEE);
	
	Entity *e = this->infront(5.0f);	// TODO: settings (melee range)
	if (e == NULL) return;
	
	DEBUG("weap", "Ray hit %p", e);
	
	if (e->klass() == UNIT) {
		((Unit*)e)->takeDamage(10);		// TODO: settings (melee damage)
	}
}

void Unit::specialAttack()
{
	// TODO: Code this one
	this->st->hud->addAlertMessage("TODO: special attack");
}


/**
* Pick up a weapon
**/
bool Unit::pickupWeapon(WeaponType* wt)
{
	if (this->pickupAmmo(wt)) return true;
	
	UnitWeapon *uw = new UnitWeapon();
	uw->wt = wt;
	uw->magazine = wt->magazine_limit;
	uw->belt = wt->belt_limit;
	uw->next_use = st->game_time;
	uw->reloading = false;
	
	this->avail_weapons.push_back(uw);
	
	this->setWeapon(this->avail_weapons.size() - 1);
	
	return true;
}

/**
* Pick up ammo
**/
bool Unit::pickupAmmo(WeaponType* wt)
{
	for (unsigned int i = 0; i < this->avail_weapons.size(); i++) {
		if (this->avail_weapons[i]->wt == wt) {
			this->avail_weapons[i]->belt += wt->belt_limit;
			this->avail_weapons[i]->belt = min(this->avail_weapons[i]->belt, wt->belt_limit);
			return true;
		}
	}
	
	return false;
}

unsigned int Unit::getNumWeapons()
{
	return this->avail_weapons.size();
}

WeaponType * Unit::getWeaponTypeAt(unsigned int id)
{
	return this->avail_weapons[id]->wt;
}

UnitWeapon * Unit::getWeaponAt(unsigned int id)
{
	return this->avail_weapons[id];
}

/**
* Sets the unit weapon
* id is the weapon id of the units personal weapon cache - not the weapontype id
**/
void Unit::setWeapon(int id)
{
	UnitWeapon *uw = this->avail_weapons.at(id);
	
	this->weapon = uw;
	this->firing = false;
	
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
* The amount of ammo in the belt, or -1 if no current weapon
**/
int Unit::getBelt()
{
	if (this->weapon == NULL) return -1;
	return this->weapon->belt;
}

/**
* The amount of ammo in the magazine, or -1 no current weapon, or -2 if reloading
**/
int Unit::getMagazine()
{
	if (this->weapon == NULL) return -1;
	if (this->weapon->reloading) return -2;
	return this->weapon->magazine;
}

/**
* Gets the unit health
**/
int Unit::getHealth()
{
	return this->health;
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
	
	
	if (st->server) st->server->addmsgUnitUpdate(this);
	
	
	// Which weapon to use?
	WeaponType *w = NULL;
	if (this->firing) {
		if (this->turret_obj) {
			w = this->st->mm->getWeaponType("poopgun");
			
		} else if (this->drive_obj) {
			w = this->st->mm->getWeaponType("rocket_launcher");
			
		} else if (this->weapon && this->weapon->next_use < st->game_time && this->weapon->magazine > 0) {
			w = this->weapon->wt;
			
		}
	}
	
	// Fire!
	if (w) {
		w->doFire(this);
		
		if (w == this->weapon->wt) {
			this->weapon->next_use = st->game_time + this->weapon->wt->fire_delay;
			
			this->weapon->magazine--;
			if (this->weapon->magazine == 0 && this->weapon->belt > 0) {
				int load = this->weapon->wt->magazine_limit;
				if (load > this->weapon->belt) load = this->weapon->belt;
				this->weapon->magazine = load;
				this->weapon->belt -= load;
				this->weapon->next_use += this->weapon->wt->reload_delay;
				this->weapon->reloading = true;
				this->endFiring();
			}
		}
		
		if (! w->continuous) this->endFiring();
	}
	
	// Reset the 'reloading' flag if enough time has passed
	if (this->weapon && this->weapon->next_use < st->game_time) {
		this->weapon->reloading = false;
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
	// TODO: New Physics
	/*if (this->lift_obj) {
		this->lift_obj->x = this->x;
		this->lift_obj->y = this->y;
		this->lift_obj->z = 70;
		this->lift_obj->angle = this->angle;
		
	} else if (this->drive_obj) {
		this->drive_obj->x = pointPlusAngleX(this->x, this->angle, -2);
		this->drive_obj->y = pointPlusAngleY(this->y, this->angle, -2);
		this->drive_obj->z = this->z;
		this->drive_obj->angle = this->angle;
		
	} else if (this->turret_obj) {
		this->x = this->turret_obj->x;
		this->y = this->turret_obj->y;
		this->z = this->turret_obj->z + 5;
		this->turret_obj->angle = this->angle;
	}*/
	
	
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
	
	this->st->increaseEntropy(1);
	
	if (this->health <= 0 && remove_at == 0) {
		this->setState(UNIT_STATE_DIE);
		this->endFiring();
		
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
	
	btTransform trans;
	this->body->getMotionState()->getWorldTransform(trans);
	
	ObjectType *ot = this->curr_obj->ot;
	
	if (ot->show_message.length() != 0) {
		this->st->hud->addAlertMessage(ot->show_message);
	}
	
	if (ot->add_object.length() != 0) {
		Object *nu = new Object(this->st->mm->getObjectType(ot->add_object), this->st, trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ(), trans.getRotation().getZ());
		this->st->addObject(nu);
	}
	
	if (ot->pickup_weapon.length() != 0) {
		WeaponType *wt = this->st->mm->getWeaponType(ot->pickup_weapon);
		if (wt) {
			this->st->hud->addAlertMessage("Picked up a ", wt->title);
			this->pickupWeapon(wt);
			this->curr_obj->del = 1;
		}
	}
	
	if (ot->ammo_crate.length() != 0) {
		WeaponType *wt = this->st->mm->getWeaponType(ot->ammo_crate);
		if (wt && this->pickupAmmo(wt)) {
			this->st->hud->addAlertMessage("Picked up some ammo");
			this->curr_obj->del = 1;
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
	this->lift_obj = NULL;
}
