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


