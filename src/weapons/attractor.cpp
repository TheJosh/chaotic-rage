// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include "../rage.h"
#include "../game_state.h"
#include "../util/quadratic.h"
#include "../entity/ammo_round.h"
#include "../entity/unit/unit.h"
#include "../physics/physics_bullet.h"
#include "ghost_objects.h"
#include "weapons.h"

using namespace std;



/**
* Fires a weapon, from a specified Unit
**/
void WeaponAttractor::doFire(Unit *u, btTransform &origin, float damage_multiplier)
{
	btTransform xform = origin;
	AmmoRound* ar = new AmmoRound(u->getGameState(), xform, this, this->ammo_model, u);

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
		float forceAmt, dist, sign;
		const btAlignedObjectArray <btCollisionObject*>* pObjsInsideGhostObject;
		btVector3 unitToCenter, force;

		sign = (this->inwards ? 1.0f : -1.0f);

		pObjsInsideGhostObject = &data->ghost->getOverlappingPairs();

		// Iterate stuff and apply a force
		for (int i = 0; i < pObjsInsideGhostObject->size(); ++i) {
			btCollisionObject* co = pObjsInsideGhostObject->at(i);

			Entity* e = (Entity*) co->getUserPointer();
			if (e == NULL) continue;
			if (e->klass() != UNIT) continue;

			// Determine distance
			unitToCenter = data->ghost->getWorldTransform().getOrigin() - co->getWorldTransform().getOrigin();

			// Handle if data is invalid from the Overlapping Pair.
			// Or, if the unit is in the center and it's a repel, choose a random direction
			if (unitToCenter.isZero() || (unitToCenter.length() < 0.1f && sign < 0.0f)) {
				unitToCenter = btVector3(getRandomf(-1.0f, 1.0f), getRandomf(-1.0f, 1.0f), getRandomf(-1.0f, 1.0f));
			}

			dist = unitToCenter.length();
			if (dist >= this->range) continue;

			// We solve the Quadratic based on the distance from the OUTSIDE of the ghost
			forceAmt = this->force.solve(this->range - dist) / 1000.f * ((float)delta);

			// Calculate force vector
			force = unitToCenter;
			force.normalize();
			force *= MIN(forceAmt, dist) * sign;

			// Apply force if non-zero
			if (force.length2() != 0.0f) {
				((Unit*)e)->applyForce(force);
			}
		}
	}
}
