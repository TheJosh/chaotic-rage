// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include <btBulletDynamicsCommon.h>
#include "rage.h"

using namespace std;



#define BIT(x) (1<<(x))
enum ColissionTypes {
	COL_NOTHING = 0,		// Nothing!
	COL_GROUND = BIT(0),	// The ground
	COL_ALIVE = BIT(1),		// Alive objects
	COL_DEAD = BIT(2)		// Dead objects
};


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
		virtual void init();
		virtual void preGame();
		virtual void postGame();
		
	public:
		btRigidBody* addRigidBody(btCollisionShape* colShape, float mass, float x, float y, float z);
		btRigidBody* addRigidBody(btCollisionShape* colShape, float m, btDefaultMotionState* motionState);
		
		void markDead(btRigidBody* body);
		void delRigidBody(btRigidBody* body);
		
		void stepTime(int ms);
		void doCollisions();
		
		btDiscreteDynamicsWorld* getWorld();
		
		static void QuaternionToEulerXYZ(const btQuaternion &quat, btVector3 &euler);

	private:
		void addBoundaryPlane(btVector3 * axis, btVector3 * loc);
};




