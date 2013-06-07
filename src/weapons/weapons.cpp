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
#include "../util/quadratic.h"

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
	btQuaternion rot = xform.getRotation() * btQuaternion(btVector3(0.0, 1.0, 0.0), DEG_TO_RAD(angle));
	xform.setRotation(rot);
	
	// Begin and end vectors
	btVector3 begin = xform.getOrigin();
	btVector3 end = begin + xform.getBasis() * btVector3(0, 0, range);
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
					((Unit*)entA)->takeDamage(this->unit_damage);
				} else if (entA->klass() == WALL) {
					((Wall*)entA)->takeDamage(this->wall_damage);
				}
			}
		}
		
	} else {
		DEBUG("weap", "%p Shot; miss", u);
	}
	
	
	// Show the weapon bullets
	create_particles_weapon(u->getGameState(), &begin, &end);
}



static void heightmapCircle(Map* map, int x0, int y0, int radius, float depthadd);

/**
* Fires a weapon, from a specified Unit
**/
void WeaponDigdown::doFire(Unit *u, btTransform &origin)
{
	btTransform xform = origin;
	
	btVector3 begin = xform.getOrigin();
	btVector3 end = begin + btVector3(0.0f, -100.0f, 0.0f);
	
	//st->addDebugLine(&begin, &end);
	
	
	// Do the rayTest
	btCollisionWorld::ClosestRayResultCallback cb(begin, end);
	cb.m_collisionFilterGroup = CG_UNIT;
	cb.m_collisionFilterMask = CG_TERRAIN;
	this->st->physics->getWorld()->rayTest(begin, end, cb);
	
	if (cb.hasHit()) {
		const btRigidBody *body = btRigidBody::upcast(cb.m_collisionObject);
		if (body && !body->getUserPointer()) {
			// Hit the ground, lets dig a hole
			Map* map = u->getGameState()->map;
			int mapX = begin.x() / map->width * map->heightmap_sx;
			int mapY = begin.z() / map->height * map->heightmap_sz;

			heightmapCircle(map, mapX, mapY, this->radius, this->depth);

			u->getGameState()->render->freeHeightmap();
			u->getGameState()->render->loadHeightmap();
		}
	}
}


/**
* Used by the dig/mound weapons
**/
void heightmapCircle(Map* map, int x0, int y0, int radius, float depthadd)
{
	int x, y, d;

	for (y = -radius; y <= radius; y++) {
		for (x = -radius; x <= radius; x++) {
			d = (radius * radius) - ((x * x) + (y * y));
			if (d > 0) {
				// distance in = d
				map->heightmapAdd(x0 + x, y0 + y, depthadd * d * 0.14);
			}
		}
	}
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
	btQuaternion rot = xform.getRotation() * btQuaternion(btVector3(0.0, 1.0, 0.0), DEG_TO_RAD(angle));
	xform.setRotation(rot);
	
	// Begin and end vectors
	btVector3 begin = xform.getOrigin();
	btVector3 end = begin + xform.getBasis() * btVector3(0, 0, range);
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
					((Unit*)entA)->takeDamage(this->unit_damage);
				} else if (entA->klass() == WALL) {
					((Wall*)entA)->takeDamage(this->wall_damage);
				}
			}
		}
		
	} else {
		DEBUG("weap", "%p Shot; miss", u);
	}
	
	// Show the weapon fire
	create_particles_flamethrower(u->getGameState(), &begin, &end);
}



