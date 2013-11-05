// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include "../rage.h"
#include "../gamestate.h"
#include "../fx/newparticle.h"
#include "ghost_objects.h"
#include "weapons.h"

using namespace std;


/**
* For physics processing
**/
static void rocketTickCallback(float delta, Entity* e, void* data1, void* data2);


/**
* Fires a weapon, from a specified Unit
**/
void WeaponRocket::doFire(Unit *u, btTransform &origin)
{
	btTransform xform = origin;
	
	AmmoRound* ar = new AmmoRound(u->getGameState(), xform, this, this->model, u, 1.0f);
	
	WeaponRocketData* data = new WeaponRocketData();
	data->delay = 250;		// So the user doesn't blow themselves up
	data->detect_ghost = create_ghost(xform, 1.0f);
	data->explode_ghost = NULL;
	data->hit = false;
	ar->data = data;
	
	u->getGameState()->addAmmoRound(ar);
	st->physics->addCollisionObject(data->detect_ghost, CG_UNIT);
	
	// Aim up a little
	btQuaternion rot = xform.getRotation() * btQuaternion(btVector3(1.0f, 0.0f, 0.0f), DEG_TO_RAD(-10.0f));
	xform.setRotation(rot);
	
	btVector3 linvel = xform.getBasis() * btVector3(0.0f, 0.0f, 50.0f);
	ar->body->setLinearVelocity(linvel);

	data->cbk = st->physics->addCallback(rocketTickCallback, static_cast<Entity*>(ar), static_cast<void*>(this), static_cast<void*>(data));
}


void rocketTickCallback(float delta, Entity *e, void *data1, void *data2)
{
	AmmoRound *ar = static_cast<AmmoRound*>(e);
	WeaponRocket *rocket = static_cast<WeaponRocket*>(data1);
	WeaponRocketData *data = static_cast<WeaponRocketData*>(data2);

	if (data->detect_ghost != NULL && check_ghost_triggered_any(data->detect_ghost)) {
		data->hit = true;
	}
}


/**
* Called at regular intervals by the mine entity
**/
void WeaponRocket::entityUpdate(AmmoRound *e, int delta)
{
	WeaponRocketData* data = (WeaponRocketData*)e->data;
	
	if (data->detect_ghost != NULL) {
		data->detect_ghost->setWorldTransform(e->getTransform());
	}
	
	// Give the user a chance
	if (data->delay > 0) {
		data->delay -= delta;
		return;
	}

	// If there is something within range...
	if (data->detect_ghost != NULL && data->hit) {
		st->physics->delCollisionObject(data->detect_ghost);
		data->detect_ghost = NULL;
		
		data->explode_ghost = create_ghost(e->getTransform(), this->range);
		st->physics->addCollisionObject(data->explode_ghost, CG_AMMO);
		return;
	}
	
	// ...kaboom
	if (data->explode_ghost != NULL) {
		apply_ghost_damage(data->explode_ghost, Quadratic(0.0f, 0.0f, this->damage), this->range);
		
		create_particles_explosion(st, e->getTransform().getOrigin(), 100);

		// Remove the rocket
		st->physics->removeCallback(data->cbk);
		st->physics->delCollisionObject(data->explode_ghost);
		delete(data);
		e->del = true;
	}
}

