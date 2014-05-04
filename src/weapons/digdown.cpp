// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include "../rage.h"
#include "../game_state.h"
#include "../util/quadratic.h"
#include "../map/map.h"
#include "../fx/newparticle.h"
#include "../entity/unit.h"
#include "../render/render.h"
#include "../physics_bullet.h"
#include "../game_engine.h"
#include "weapons.h"

using namespace std;




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
			int mapX = static_cast<int>(begin.x() / map->width * map->heightmap->sx);
			int mapY = static_cast<int>(begin.z() / map->height * map->heightmap->sz);

			heightmapCircle(map, mapX, mapY, this->radius, this->depth);

			GEng()->render->freeHeightmap();
			GEng()->render->loadHeightmap();
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
				map->heightmapAdd(x0 + x, y0 + y, depthadd * d * 0.14f);
			}
		}
	}
}


