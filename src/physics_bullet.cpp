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
	
	
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,0,1),1);
	
	
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,0,-1)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(
		0,
		groundMotionState,
		groundShape,
		btVector3(0,0,0)
	);
	this->groundRigidBody = new btRigidBody(groundRigidBodyCI);
	dynamicsWorld->addRigidBody(groundRigidBody);
	
	
	cout << "\n\ngroundRigidBody: " << groundRigidBody << "\n";
	
	
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
* Returns the world
**/
btDiscreteDynamicsWorld* PhysicsBullet::getWorld()
{
	return dynamicsWorld;
}


/**
* Create and add a rigid body
*
* m = mass
* x,y,z = origin position
**/
btRigidBody* PhysicsBullet::addRigidBody(btCollisionShape* colShape, float m, float x, float y, float z)
{
	btDefaultMotionState* myMotionState =
		new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(x,y,z)));
	
	btScalar mass(m);
	bool isDynamic = (mass != 0.f);
	
	btVector3 localInertia(0,0,0);
	if (isDynamic)
		colShape->calculateLocalInertia(mass,localInertia);
	
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


/**
* Remove a rigid body from the game world
**/
void PhysicsBullet::delRigidBody(btRigidBody* body)
{
	if (body && body->getMotionState()) {
		delete body->getMotionState();
	}
	
	dynamicsWorld->removeCollisionObject(body);
	
	delete body;
}


/**
* Step the physics forward by the given amount of time
**/
void PhysicsBullet::stepTime(int ms)
{
	dynamicsWorld->stepSimulation(ms / 1000.0f, 10);
}



/**
* Look for collissions of entities
**/
void PhysicsBullet::doCollisions()
{
	int i;
	
	int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
	
	DEBUG("coll", "Num manifolds: %i", numManifolds);
	
	for (i = 0; i < numManifolds; i++) {
		btPersistentManifold* contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		
		btCollisionObject* obA = static_cast<btCollisionObject*>(contactManifold->getBody0());
		btCollisionObject* obB = static_cast<btCollisionObject*>(contactManifold->getBody1());
		
		if (obA == this->groundRigidBody || obB == this->groundRigidBody) continue;
		
		DEBUG("coll", "Collision: %p %p", obA, obB);
		
		Entity* entA = static_cast<Entity*>(obA->getUserPointer());
		Entity* entB = static_cast<Entity*>(obB->getUserPointer());
		
		entA->hasBeenHit(entB);
		entB->hasBeenHit(entA);
		
		/*int numContacts = contactManifold->getNumContacts();
		
		for (j = 0; j < numContacts; j++) {
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			
			glBegin(GL_LINES);
			glColor3f(0, 0, 0);
			
			btVector3 ptA = pt.getPositionWorldOnA();
			btVector3 ptB = pt.getPositionWorldOnB();
			
			glVertex3d(ptA.x(),ptA.y(),ptA.z());
			glVertex3d(ptB.x(),ptB.y(),ptB.z());
			glEnd();
		}*/
		
		//you can un-comment out this line, and then all points are removed
		//contactManifold->clearManifold();	
	}
}




