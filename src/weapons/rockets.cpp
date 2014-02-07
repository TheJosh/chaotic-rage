// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include "../rage.h"
#include "../game_state.h"
#include "../fx/newparticle.h"
#include "../physics_bullet.h"
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
	xform.setOrigin(
		xform.getOrigin() + xform.getBasis() * btVector3(0.0f, 0.5f, 2.0f)
	);
	
	// Create ammo obj
	AmmoRound* ar = new AmmoRound(u->getGameState(), xform, this, this->model, u, 1.0f);
	
	// Create ghost for detection
	btPairCachingGhostObject* ghost = new btPairCachingGhostObject();
	btSphereShape* shape = new btSphereShape(1.0f);
	ghost->setWorldTransform(xform);
	ghost->setCollisionShape(shape);
	
	// Forwards motion
	ar->body->setLinearVelocity(
		xform.getBasis() * btVector3(0.0f, 0.0f, 50.0f)
	);
	
	// Create and attach data
	WeaponRocketData* data = new WeaponRocketData();
	data->state = 0;
	data->ghost = ghost;
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
			{
				btManifoldArray manifoldArray;
				btBroadphasePairArray& pairArray = static_cast<btPairCachingGhostObject*>(rocket->ghost)->getOverlappingPairCache()->getOverlappingPairArray();
				int numPairs = pairArray.size();
				
				bool hit = false;
				for (int i=0;i<numPairs;i++) {
					const btBroadphasePair& pair = pairArray[i];

					//unless we manually perform collision detection on this pair, the contacts are in the dynamics world paircache:
					btBroadphasePair* collisionPair = wt->st->physics->getWorld()->getPairCache()->findPair(pair.m_pProxy0, pair.m_pProxy1);
					if (!collisionPair) continue;

					manifoldArray.clear();
					if (collisionPair->m_algorithm) {
						collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);
					}
				
					for (int j=0;j<manifoldArray.size();j++) {
						btPersistentManifold* manifold = manifoldArray[j];
						for (int p=0;p<manifold->getNumContacts();p++) {
							const btManifoldPoint&pt = manifold->getContactPoint(p);
						
							if (pt.getDistance() < 0.f) {
								hit = true;
								break;
							}
						}
					}
				}
				
				// hit - explode
				if (hit) {
					rocket->state = 1;
				
					wt->st->physics->delCollisionObject(rocket->ghost);
				
					rocket->ghost = create_ghost(ar->getTransform(), wt->range);
					wt->st->physics->addCollisionObject(rocket->ghost, CG_AMMO);
				}
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

