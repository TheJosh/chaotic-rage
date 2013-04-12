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

using namespace std;


btGhostObject* create_ghost(Entity* e, float radius);
void apply_ghost_damage(btGhostObject* ghost, float damage);


/**
* Fires a weapon, from a specified Unit
**/
void WeaponTimedMine::doFire(Unit *u, btTransform &origin)
{
	btTransform xform = origin;	
	AmmoRound* ar = new AmmoRound(u->getGameState(), xform, this, this->model);
	
	WeaponTimedMineData* data = new WeaponTimedMineData();
	data->time = this->time;
	data->ghost = NULL;
	ar->data = data;
	
	u->getGameState()->addAmmoRound(ar);
}


/**
* Called at regular intervals by the mine entity
**/
void WeaponTimedMine::entityUpdate(AmmoRound *e, int delta)
{
	WeaponTimedMineData* data = (WeaponTimedMineData*)e->data;
	
	// If there is a ghost, see what we hit
	if (data->ghost) {
		apply_ghost_damage(data->ghost, 1000.0f);
		
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
		data->ghost = create_ghost(e, 10.0f);
		st->physics->addCollisionObject(data->ghost, CG_AMMO);

		create_particles_explosion(st, e->getTransform().getOrigin(), 100);
	}
}


/**
* Create a chost object for collision detection
**/
btGhostObject* create_ghost(Entity* e, float radius)
{
	btGhostObject* ghost = new btGhostObject();

	ghost->setWorldTransform(e->getTransform());
	ghost->setCollisionFlags(ghost->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

	btSphereShape* shape = new btSphereShape(radius);
	ghost->setCollisionShape(shape);

	return ghost;
}


/**
* If there is anything within the ghost radius, apply damage as appropriate
**/
void apply_ghost_damage(btGhostObject* ghost, float damage)
{
	const btAlignedObjectArray <btCollisionObject*>* pObjsInsideGhostObject;
	pObjsInsideGhostObject = &ghost->getOverlappingPairs();
		
	for (int i = 0; i < pObjsInsideGhostObject->size(); ++i) {
		btCollisionObject* co = pObjsInsideGhostObject->at(i);
			
		Entity* e = (Entity*) co->getUserPointer();
		if (e == NULL) continue;
			
		if (e->klass() == UNIT) {
			((Unit*)e)->takeDamage(damage);
		} else if (e->klass() == WALL) {
			((Wall*)e)->takeDamage(damage);
		}
	}
}
