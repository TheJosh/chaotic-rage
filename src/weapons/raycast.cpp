// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include "../game_engine.h"
#include "../util/quadratic.h"
#include "../util/debug.h"
#include "../util/util.h"
#include "../fx/effects_manager.h"
#include "../physics/physics_bullet.h"
#include "../entity/unit/unit.h"
#include "../entity/wall.h"
#include "weapons.h"

using namespace std;



/**
* Transforms of the raycast weapons
**/
template <class T>
void raycastDoFire(T const &weapon, Unit *u, btTransform &origin, btVector3 &begin, btVector3 &end, float damage_multiplier)
{
	btTransform xform = origin;

	// Weapon angle ranges
	int angle = weapon->angle_range / 2;
	btQuaternion rot = xform.getRotation()
		* btQuaternion(btVector3(0.0f, 1.0f, 0.0f), DEG_TO_RAD(getRandomf(-angle, angle)))
		* btQuaternion(btVector3(1.0f, 0.0f, 0.0f), DEG_TO_RAD(getRandomf(-angle, angle)));
	xform.setRotation(rot);

	// Begin and end vectors
	begin = xform.getOrigin();
	end = begin + xform.getBasis() * btVector3(0.0f, 0.0f, weapon->range);

	// Do the rayTest
	btCollisionWorld::ClosestRayResultCallback cb(begin, end);
	cb.m_collisionFilterGroup = CG_UNIT;
	cb.m_collisionFilterMask = PhysicsBullet::mask_entities;
	weapon->st->physics->getWorld()->rayTest(begin, end, cb);

	if (!cb.hasHit() || cb.m_collisionObject->getUserPointer() == NULL) {
		return;
	}

	btVector3 dist_vec = cb.m_hitPointWorld - begin;
	float dist = dist_vec.length();
	float damage_falloff;

	if (dist < 3.0f) {
		damage_falloff = 1.0f;
	} else {
		damage_falloff = sqrt(weapon->range - dist) / sqrt(weapon->range);
	}

	Entity* entA = static_cast<Entity*>(cb.m_collisionObject->getUserPointer());
	DEBUG("weap", "Ray hit %p", entA);

	entA->takeDamage(weapon->damage * damage_falloff * damage_multiplier);
}


/**
* Fires a weapon, from a specified Unit
**/
void WeaponRaycast::doFire(Unit *u, btTransform &origin, float damage_multiplier)
{
	btVector3 begin;
	btVector3 end;

	for (unsigned int i = this->burst; i != 0; --i) {
		raycastDoFire(this, u, origin, begin, end, damage_multiplier);
		st->effects->weaponBullets(&begin, &end, this->speed);
	}
}


/**
* Fires a weapon, from a specified Unit
**/
void WeaponFlamethrower::doFire(Unit *u, btTransform &origin, float damage_multiplier)
{
	btVector3 begin;
	btVector3 end;

	for (unsigned int i = this->burst; i != 0; --i) {
		raycastDoFire(this, u, origin, begin, end, damage_multiplier);

		// Camera is brought up to eye level
		begin += btVector3(0.0f, UNIT_PHYSICS_HEIGHT / 2.3f, 0.0f);

		// Location of weapon relative to camera (see also RenderOpenGL::weapon)
		btVector3 localOffset = btVector3(-0.15f, -0.3f, 0.15f);

		// Location of barrel tip
		glm::vec3 barrel = attach_loc[WPATT_BARREL];
		localOffset += btVector3(barrel.x, barrel.y, barrel.z);

		btVector3 worldOffset = begin + (origin.getBasis() * localOffset);
		st->effects->weaponFlamethrower(&worldOffset, &end);
	}
}
