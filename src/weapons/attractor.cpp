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
#include "../util/quadratic.h"

using namespace std;



/**
* Fires a weapon, from a specified Unit
**/
void WeaponAttractor::doFire(Unit *u, btTransform &origin)
{
	btTransform xform = origin;	
	AmmoRound* ar = new AmmoRound(u->getGameState(), xform, this, this->model, u);
	
	WeaponAttractorData* data = new WeaponAttractorData();
	data->destroy_time = u->getGameState()->game_time + this->time;
	data->ghost = create_ghost(xform, this->range);
	ar->data = data;
	
	u->getGameState()->addAmmoRound(ar);
	st->physics->addCollisionObject(data->ghost, CG_AMMO);
}


/**
* Called at regular intervals by the attractor entity
**/
void WeaponAttractor::entityUpdate(AmmoRound *e, int delta)
{
	WeaponAttractorData* data = (WeaponAttractorData*)e->data;
	
	// Remove the attractor after it's too old
	if (data->destroy_time < e->getGameState()->game_time) {
		e->getGameState()->physics->delCollisionObject(data->ghost);
		delete(data);
		e->del = true;
		return;
	}

	// Apply a force to anything within range
	{
		float dist, force;
		const btAlignedObjectArray <btCollisionObject*>* pObjsInsideGhostObject;
		
		pObjsInsideGhostObject = &data->ghost->getOverlappingPairs();
		
		// Iterate stuff and apply a force
		for (int i = 0; i < pObjsInsideGhostObject->size(); ++i) {
			btCollisionObject* co = pObjsInsideGhostObject->at(i);
			
			Entity* e = (Entity*) co->getUserPointer();
			if (e == NULL) continue;
			if (e->klass() != UNIT) continue;
			
			// Determine distance
			dist = data->ghost->getWorldTransform().getOrigin().distance(co->getWorldTransform().getOrigin());
			if (dist >= this->range) continue;
			
			// We solve the Quadratic based on the distance from the OUTSIDE of the ghost
			force = this->force.solve(this->range - dist);
			
			// TODO: apply a force
			
			// The code ight look something like this:
			//((Unit*)e)->applyForceTowards(data->ghost->getWorldTransform().getOrigin(), force);
		}
	}
}

