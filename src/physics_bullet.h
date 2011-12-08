// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include <btBulletDynamicsCommon.h>
#include "rage.h"

using namespace std;


/**
* Thin wrapper around the "Bullet" physics library
**/
class PhysicsBullet
{
	private:
		GameState * st;
		
		btDefaultCollisionConfiguration* collisionConfiguration;
		btCollisionDispatcher* dispatcher;
		btBroadphaseInterface* overlappingPairCache;
		btSequentialImpulseConstraintSolver* solver;
		btDiscreteDynamicsWorld* dynamicsWorld;
		btAlignedObjectArray<btCollisionShape*>* collisionShapes;
		
		btRigidBody* groundRigidBody;
		
	public:
		PhysicsBullet(GameState * st);
		~PhysicsBullet();
		virtual void preGame();
		virtual void postGame();
		
	public:
		btRigidBody* addRigidBody(btCollisionShape* colShape, float mass, float x, float y, float z);
		void delRigidBody(btRigidBody* body);
		
		void stepTime(int ms);
		void doCollisions();
};




