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
		
		
	public:
		virtual void preGame();
		virtual void postGame();
		
	public:
		PhysicsBullet(GameState * st);
		~PhysicsBullet();
};




