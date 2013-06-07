// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include "../rage.h"
#include "ghost_objects.h"

using namespace std;



/**
* Fires a weapon, from a specified Unit
**/
void WeaponRocket::doFire(Unit *u, btTransform &origin)
{
	btTransform xform = origin;
	
	AmmoRound* ar = new AmmoRound(u->getGameState(), xform, this, this->model, u, 0.1f);
	
	WeaponRocketData* data = new WeaponRocketData();
	data->delay = 250;		// So the user doesn't blow themselves up
	data->ghost = create_ghost(xform, this->range);
	ar->data = data;
	
	u->getGameState()->addAmmoRound(ar);
	st->physics->addCollisionObject(data->ghost, CG_AMMO);
	
	// TODO: Angle range
	/*int angle = this->angle_range / 2;
	angle = getRandom(-angle, angle);
	btQuaternion rot = xform.getRotation() * btQuaternion(btVector3(0.0, 1.0, 0.0), DEG_TO_RAD(angle));
	xform.setRotation(rot);*/
	
	btVector3 linvel = xform.getBasis() * btVector3(0, 0, 50.0f);
	ar->body->setLinearVelocity(linvel);
}


/**
* Called at regular intervals by the mine entity
**/
void WeaponRocket::entityUpdate(AmmoRound *e, int delta)
{
	WeaponRocketData* data = (WeaponRocketData*)e->data;
	
	data->ghost->setWorldTransform(e->getTransform());
	
	// Give the user a chance
	if (data->delay > 0) {
		data->delay -= delta;
		return;
	}

	// If there is something within range...
	if (check_ghost_triggered(data->ghost)) {
		// ...kaboom
		apply_ghost_damage(data->ghost, Quadratic(0.0f, 0.0f, this->damage), this->range);
		
		create_particles_explosion(st, e->getTransform().getOrigin(), 100);

		// Remove the rocket
		e->getGameState()->physics->delCollisionObject(data->ghost);
		delete(data);
		e->del = true;
	}
}

