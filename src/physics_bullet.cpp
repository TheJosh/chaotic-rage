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



