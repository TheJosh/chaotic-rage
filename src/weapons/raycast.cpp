// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include "../rage.h"
#include "../game_state.h"
#include "../util/quadratic.h"
#include "../fx/newparticle.h"
#include "../physics_bullet.h"
#include "../entity/unit.h"
#include "../entity/wall.h"
#include "weapons.h"

using namespace std;



/**
* Transforms of the raycast weapons
**/
template <class T>
void raycastDoFire(T const &weapon, Unit *u, btTransform &origin, btVector3 &begin, btVector3 &end)
{
	btTransform xform = origin;

	// Weapon angle ranges
	int angle = weapon->angle_range / 2;
	angle = getRandom(-angle, angle);
	btQuaternion rot = xform.getRotation() * btQuaternion(btVector3(0.0f, 1.0f, 0.0f), DEG_TO_RAD(angle));
	xform.setRotation(rot);

	// Begin and end vectors
	begin = xform.getOrigin();
	end = begin + xform.getBasis() * btVector3(0.0f, 0.0f, weapon->range);
	weapon->st->addDebugLine(&begin, &end);

	// Do the rayTest
	btCollisionWorld::ClosestRayResultCallback cb(begin, end);
	cb.m_collisionFilterGroup = CG_UNIT;
	cb.m_collisionFilterMask = PhysicsBullet::mask_entities;
	weapon->st->physics->getWorld()->rayTest(begin, end, cb);

	if (cb.hasHit()) {
		if (cb.m_collisionObject->getUserPointer()) {
			Entity* entA = static_cast<Entity*>(cb.m_collisionObject->getUserPointer());
			DEBUG("weap", "Ray hit %p", entA);
			if (entA) {
				if (entA->klass() == UNIT) {
					((Unit*)entA)->takeDamage(weapon->damage);
				} else if (entA->klass() == WALL) {
					((Wall*)entA)->takeDamage(weapon->damage);
				}
			}
		}

	} else {
		DEBUG("weap", "%p Shot; miss", u);
	}
}


/**
* Fires a weapon, from a specified Unit
**/
void WeaponRaycast::doFire(Unit *u, btTransform &origin)
{
	btVector3 begin;
	btVector3 end;

	for (unsigned int i = this->burst; i != 0; --i) {
		raycastDoFire(this, u, origin, begin, end);
		create_particles_weapon(u->getGameState(), &begin, &end);
	}
}


/**
* Fires a weapon, from a specified Unit
**/
void WeaponFlamethrower::doFire(Unit *u, btTransform &origin)
{
	btVector3 begin;
	btVector3 end;

	raycastDoFire(this, u, origin, begin, end);

	// Show the weapon fire
	create_particles_flamethrower(u->getGameState(), &begin, &end);
}
