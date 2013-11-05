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
	
	// Aim up a little
	btQuaternion rot = xform.getRotation() * btQuaternion(btVector3(1.0f, 0.0f, 0.0f), DEG_TO_RAD(-10.0f));
	xform.setRotation(rot);
	
	// Move forwards a little
	xform.setOrigin(xform.getOrigin() + xform.getBasis() * btVector3(0.0f, 0.0f, 10.0f));
	
	// Create ammo obj
	AmmoRound* ar = new AmmoRound(u->getGameState(), xform, this, this->model, u, 1.0f);
	
	// Forwards motion
	btVector3 linvel = xform.getBasis() * btVector3(0.0f, 0.0f, 50.0f);
	ar->body->setLinearVelocity(linvel);
	
	// Create and attach data
	WeaponRocketData* data = new WeaponRocketData();
	data->state = 0;
	data->ghost = create_ghost(xform, 1.0f);
	ar->data = data;
	
	// Insert into physics world
	this->st->physics->addCollisionObject(data->ghost, CG_UNIT);
	this->st->addAmmoRound(ar);
	data->cbk = this->st->physics->addCallback(rocketTickCallback, static_cast<Entity*>(ar), static_cast<void*>(this), static_cast<void*>(data));
}


void rocketTickCallback(float delta, Entity *e, void *data1, void *data2)
{
	AmmoRound *ar = static_cast<AmmoRound*>(e);
	WeaponRocket *wt = static_cast<WeaponRocket*>(data1);
	WeaponRocketData *rocket = static_cast<WeaponRocketData*>(data2);
	
	rocket->ghost->setWorldTransform(ar->getTransform());
	
	switch (rocket->state) {
		case 0:
			// Flying through the air - check for collision
			if (check_ghost_triggered_any(rocket->ghost)) {
				rocket->state = 1;
				
				wt->st->physics->delCollisionObject(rocket->ghost);
				
				rocket->ghost = create_ghost(ar->getTransform(), wt->range);
				wt->st->physics->addCollisionObject(rocket->ghost, CG_AMMO);
			}
			break;
			
		case 1:
			// We've hit something - kaboom
			apply_ghost_damage(rocket->ghost, Quadratic(0.0f, 0.0f, wt->damage), wt->range);
			create_particles_explosion(wt->st, ar->getTransform().getOrigin(), 100);
			
			// Remove the rocket
			wt->st->physics->removeCallback(rocket->cbk);
			wt->st->physics->delCollisionObject(rocket->ghost);
			delete(rocket);
			ar->del = true;
			break;
	}
}


/**
* Called at regular intervals by the mine entity
**/
void WeaponRocket::entityUpdate(AmmoRound *e, int delta)
{
	return;
}

