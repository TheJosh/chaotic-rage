// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include "../rage.h"
#include "../game_state.h"
#include "../util/quadratic.h"
#include "../map.h"
#include "../fx/newparticle.h"
#include "weapons.h"

using namespace std;



/**
* Fires a weapon, from a specified Unit
**/
void WeaponRaycast::doFire(Unit *u, btTransform &origin)
{
	btTransform xform = origin;
	
	// Weapon angle ranges
	int angle = this->angle_range / 2;
	angle = getRandom(-angle, angle);
	btQuaternion rot = xform.getRotation() * btQuaternion(btVector3(0.0f, 1.0f, 0.0f), DEG_TO_RAD(angle));
	xform.setRotation(rot);
	
	// Begin and end vectors
	btVector3 begin = xform.getOrigin();
	btVector3 end = begin + xform.getBasis() * btVector3(0.0f, 0.0f, range);
	st->addDebugLine(&begin, &end);
	
	// Do the rayTest
	btCollisionWorld::ClosestRayResultCallback cb(begin, end);
	cb.m_collisionFilterGroup = CG_UNIT;
	cb.m_collisionFilterMask = PhysicsBullet::mask_entities;
	this->st->physics->getWorld()->rayTest(begin, end, cb);
	
	if (cb.hasHit()) {
		if (cb.m_collisionObject->getUserPointer()) {
			Entity* entA = static_cast<Entity*>(cb.m_collisionObject->getUserPointer());
			DEBUG("weap", "Ray hit %p", entA);
			if (entA) {
				if (entA->klass() == UNIT) {
					((Unit*)entA)->takeDamage(this->damage);
				} else if (entA->klass() == WALL) {
					((Wall*)entA)->takeDamage(this->damage);
				}
			}
		}
		
	} else {
		DEBUG("weap", "%p Shot; miss", u);
	}
	
	
	// Show the weapon bullets
	create_particles_weapon(u->getGameState(), &begin, &end);
}



/**
* Fires a weapon, from a specified Unit
**/
void WeaponFlamethrower::doFire(Unit *u, btTransform &origin)
{
	btTransform xform = origin;
	
	// Weapon angle ranges
	int angle = this->angle_range / 2;
	angle = getRandom(-angle, angle);
	btQuaternion rot = xform.getRotation() * btQuaternion(btVector3(0.0f, 1.0f, 0.0f), DEG_TO_RAD(angle));
	xform.setRotation(rot);
	
	// Begin and end vectors
	btVector3 begin = xform.getOrigin();
	btVector3 end = begin + xform.getBasis() * btVector3(0.0f, 0.0f, range);
	st->addDebugLine(&begin, &end);
	
	// Do the rayTest
	btCollisionWorld::ClosestRayResultCallback cb(begin, end);
	cb.m_collisionFilterGroup = CG_UNIT;
	cb.m_collisionFilterMask = PhysicsBullet::mask_entities;
	this->st->physics->getWorld()->rayTest(begin, end, cb);
	
	if (cb.hasHit()) {
		if (cb.m_collisionObject->getUserPointer()) {
			Entity* entA = static_cast<Entity*>(cb.m_collisionObject->getUserPointer());
			DEBUG("weap", "Ray hit %p", entA);
			if (entA) {
				if (entA->klass() == UNIT) {
					((Unit*)entA)->takeDamage(this->damage);
				} else if (entA->klass() == WALL) {
					((Wall*)entA)->takeDamage(this->damage);
				}
			}
		}
		
	} else {
		DEBUG("weap", "%p Shot; miss", u);
	}
	
	// Show the weapon fire
	create_particles_flamethrower(u->getGameState(), &begin, &end);
}



