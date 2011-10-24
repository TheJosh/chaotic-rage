// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <algorithm>
#include <SDL.h>
#include <btBulletDynamicsCommon.h>
#include "rage.h"

using namespace std;



PhysicsBullet::PhysicsBullet(GameState * st)
{
	this->st = st;
	st->physics = this;
}

PhysicsBullet::~PhysicsBullet()
{
}



/**
* Set up the physics world
**/
void PhysicsBullet::preGame()
{
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	overlappingPairCache = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
	
	dynamicsWorld = new btDiscreteDynamicsWorld(
		dispatcher,
		overlappingPairCache,
		solver,
		collisionConfiguration
	);
	
	dynamicsWorld->setGravity(btVector3(0,0,-10));
	
	collisionShapes = new btAlignedObjectArray<btCollisionShape*>();
}


/**
* Tear down the physics world
**/
void PhysicsBullet::postGame()
{
	// TODO: Free other stuff properly, see HelloWorld demo
	
	delete collisionShapes;
	delete dynamicsWorld;
	delete solver;
	delete overlappingPairCache;
	delete dispatcher;
	delete collisionConfiguration;
}


/**
* Create and add a rigid body
*
* m = mass
* x,y,z = origin position
**/
btRigidBody* PhysicsBullet::addRigidBody(btCollisionShape* colShape, float m, float x, float y, float z)
{
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(x, y, z));
	
	btScalar mass(m);
	bool isDynamic = (mass != 0.f);
	
	btVector3 localInertia(0,0,0);
	if (isDynamic)
		colShape->calculateLocalInertia(mass,localInertia);
	
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(
		mass,
		myMotionState,
		colShape,
		localInertia
	);
	btRigidBody* body = new btRigidBody(rbInfo);
	
	dynamicsWorld->addRigidBody(body);
	
	return body;
}



