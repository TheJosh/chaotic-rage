// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include "../game_engine.h"
#include "../fx/effects_manager.h"
#include "../physics/physics_bullet.h"
#include "../entity/ammo_round.h"
#include "../entity/unit/unit.h"
#include "ghost_objects.h"
#include "weapons.h"

using namespace std;




/**
* Fires a weapon, from a specified Unit
**/
void WeaponTimedMine::doFire(Unit *u, btTransform &origin, float damage_multiplier)
{
	btTransform xform = origin;
	AmmoRound* ar = new AmmoRound(u->getGameState(), xform, this, this->ammo_model, u, 1.0f);

	btVector3 throwing = xform.getBasis() * btVector3(0.0f, 0.0f, 10.0f);
	ar->body->applyCentralImpulse(throwing);

	WeaponTimedMineData* data = new WeaponTimedMineData();
	data->time = this->time;
	data->ghost = NULL;
	data->damage = this->damage * damage_multiplier;
	ar->data = data;

	u->getGameState()->addAmmoRound(ar);

	btVector3 linvel = xform.getBasis() * btVector3(0.0f, 0.0f, 5.0f);
	ar->body->setLinearVelocity(linvel);
}


/**
* Called at regular intervals by the mine entity
**/
void WeaponTimedMine::entityUpdate(AmmoRound *e, int delta)
{
	WeaponTimedMineData* data = (WeaponTimedMineData*)e->data;
	if (!data) return;

	// If there is a ghost, see what we hit
	if (data->ghost) {
		apply_ghost_damage(data->ghost, Quadratic(0.0f, data->damage, 0.0f), this->range);

		// Remove the mine
		e->getGameState()->physics->delCollisionObject(data->ghost);
		delete(data);
		e->del = true;

		return;
	}

	data->time -= delta;

	// If the time has passed, it's boom time
	// We create a ghost, and apply the damage in the next tick.
	if (data->time <= 0) {
		data->ghost = create_ghost(e->getTransform(), this->range);
		st->physics->addCollisionObject(data->ghost, CG_AMMO);
		st->effects->explosion(e->getTransform().getOrigin(), 100);
	}
}



/**
* Fires a weapon, from a specified Unit
**/
void WeaponProxiMine::doFire(Unit *u, btTransform &origin, float damage_multiplier)
{
	btTransform xform = origin;
	AmmoRound* ar = new AmmoRound(u->getGameState(), xform, this, this->ammo_model, u, 1.0f);

	btVector3 throwing = xform.getBasis() * btVector3(0.0f, 0.0f, 10.0f);
	ar->body->applyCentralImpulse(throwing);

	WeaponProxiMineData* data = new WeaponProxiMineData();
	data->delay = 2000;
	data->ghost = create_ghost(xform, this->range);
	data->damage = this->damage * damage_multiplier;
	ar->data = data;

	u->getGameState()->addAmmoRound(ar);
	st->physics->addCollisionObject(data->ghost, CG_AMMO);
}


/**
* Called at regular intervals by the mine entity
**/
void WeaponProxiMine::entityUpdate(AmmoRound *e, int delta)
{
	WeaponProxiMineData* data = (WeaponProxiMineData*)e->data;
	if (!data) return;

	data->ghost->setWorldTransform(e->body->getWorldTransform());

	// Give the user a chance
	if (data->delay > 0) {
		data->delay -= delta;
		return;
	}

	// If there is something within range...
	if (check_ghost_triggered_units(data->ghost)) {
		// ...kaboom
		apply_ghost_damage(data->ghost, Quadratic(0.0f, data->damage, 0.0f), this->range);

		st->effects->explosion(e->getTransform().getOrigin(), 100);

		// Remove the mine
		e->getGameState()->physics->delCollisionObject(data->ghost);
		delete(data);
		e->del = true;
	}
}



/**
* Fires a weapon, from a specified Unit
**/
void WeaponRemoteMine::doFire(Unit *u, btTransform &origin, float damage_multiplier)
{
	btTransform xform = origin;
	AmmoRound* ar = new AmmoRound(u->getGameState(), xform, this, this->ammo_model, u, 1.0f);

	btVector3 throwing = xform.getBasis() * btVector3(0.0f, 0.0f, 10.0f);
	ar->body->applyCentralImpulse(throwing);

	WeaponRemoteMineData* data = new WeaponRemoteMineData();
	data->ghost = NULL;
	data->damage = this->damage * damage_multiplier;
	ar->data = data;

	u->getGameState()->addAmmoRound(ar);
}


/**
* Called at regular intervals by the mine entity
**/
void WeaponRemoteMine::entityUpdate(AmmoRound *e, int delta)
{
	WeaponRemoteMineData* data = (WeaponRemoteMineData*)e->data;
	if (!data || !data->ghost) return;

	// If there is a ghost, we have just boomed, let's see what we hit
	apply_ghost_damage(data->ghost, Quadratic(0.0f, data->damage, 0.0f), this->range);

	// Remove the mine
	e->getGameState()->physics->delCollisionObject(data->ghost);
	delete(data);
	e->del = true;
}


/**
* It's boom time
**/
void WeaponRemoteMine::trigger(AmmoRound *e)
{
	WeaponRemoteMineData* data = (WeaponRemoteMineData*)e->data;

	data->ghost = create_ghost(e->getTransform(), this->range);
	st->physics->addCollisionObject(data->ghost, CG_AMMO);
	st->effects->explosion(e->getTransform().getOrigin(), 100);
}


/**
* Trigger remote mines
**/
void WeaponRemoteTrigger::doFire(Unit *u, btTransform &origin, float damage_multiplier)
{
	list<AmmoRound*>* rounds = u->getGameState()->findAmmoRoundsUnit(u);

	for (list<AmmoRound*>::iterator it = rounds->begin(); it != rounds->end(); ++it) {
		AmmoRound* ar = (*it);
		if (ar->wt->type == WEAPON_TYPE_REMOTE_MINE) {
			((WeaponRemoteMine*)ar->wt)->trigger(ar);
		}
	}

	delete(rounds);
}
