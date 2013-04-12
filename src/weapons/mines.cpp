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




/**
* Create a chost object for collision detection
**/
btGhostObject* create_ghost(btTransform& xform, float radius)
{
	btGhostObject* ghost = new btGhostObject();

	ghost->setWorldTransform(xform);
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


/**
* If there is anything within the ghost radius, apply damage as appropriate
**/
bool check_ghost_triggered(btGhostObject* ghost)
{
	const btAlignedObjectArray <btCollisionObject*>* pObjsInsideGhostObject;
	pObjsInsideGhostObject = &ghost->getOverlappingPairs();
		
	for (int i = 0; i < pObjsInsideGhostObject->size(); ++i) {
		btCollisionObject* co = pObjsInsideGhostObject->at(i);
			
		Entity* e = (Entity*) co->getUserPointer();
		if (e == NULL) continue;
			
		if (e->klass() == UNIT) {
			return true;
		}
	}

	return false;
}



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
		data->ghost = create_ghost(e->getTransform(), 10.0f);
		st->physics->addCollisionObject(data->ghost, CG_AMMO);

		create_particles_explosion(st, e->getTransform().getOrigin(), 100);
	}
}



/**
* Fires a weapon, from a specified Unit
**/
void WeaponProxiMine::doFire(Unit *u, btTransform &origin)
{
	btTransform xform = origin;	
	AmmoRound* ar = new AmmoRound(u->getGameState(), xform, this, this->model);
	
	WeaponProxiMineData* data = new WeaponProxiMineData();
	data->delay = 2000;
	data->ghost = create_ghost(xform, 10.0f);
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
	
	// Give the user a chance
	if (data->delay > 0) {
		data->delay -= delta;
		return;
	}

	// If there is something within range...
	if (check_ghost_triggered(data->ghost)) {
		// ...kaboom
		apply_ghost_damage(data->ghost, 1000.0f);
		
		create_particles_explosion(st, e->getTransform().getOrigin(), 100);

		// Remove the mine
		e->getGameState()->physics->delCollisionObject(data->ghost);
		delete(data);
		e->del = true;
	}
}

