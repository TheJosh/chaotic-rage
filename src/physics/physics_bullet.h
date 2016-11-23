// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <list>
#include <map>
#include <btBulletDynamicsCommon.h>
#include "physics_callback.h"
class Entity;
class GameState;
class PhysicsCallback;
class btGhostPairCallback;

using namespace std;



#define BIT(x) (1<<(x))
enum CollisionGroup {
	CG_NOTHING = 0,           // Nothing!
	CG_TERRAIN = BIT(0),      // The ground surface
	CG_WATER = BIT(1),        // Water terrain, or perhaps quicksand
	CG_DEBRIS = BIT(2),       // Debris, dead stuff, etc
	CG_AMMO = BIT(3),         // AmmoRound entities
	CG_WALL = BIT(4),         // Walls and other statics
	CG_OBJECT = BIT(5),       // Objects and other dynamics
	CG_UNIT = BIT(6),         // Units (btKinematicCharacterController)
	CG_VEHICLE = BIT(7),      // Vehicles (btRaycastVehicle)
	CG_PICKUP = BIT(8),       // Pickups, Powerups, etc.
};


#if (BT_BULLET_VERSION == 282)
inline int bullet_btInfinityMask()
{
	return btInfinityMask;
}
#endif


/**
* Simple wrapper around the "Bullet" physics library
**/
class PhysicsBullet
{
	private:
		GameState * st;

		btDefaultCollisionConfiguration* collisionConfiguration;
		btCollisionDispatcher* dispatcher;
		btBroadphaseInterface* broadphase;
		btSequentialImpulseConstraintSolver* solver;
		btDiscreteDynamicsWorld* dynamicsWorld;
		btAlignedObjectArray<btCollisionShape*>* collisionShapes;
		btGhostPairCallback *ghostPairCallback;

		btRigidBody* groundRigidBody;

	public:
		// I like to keep all of my mask suff in one place (this class).
		// Various constants to achieve that goal
		std::map<int, int> masks;
		static const int mask_entities = (CG_WALL | CG_OBJECT | CG_UNIT | CG_VEHICLE);

		std::list<PhysicsCallback*> callbacks;

	public:
		explicit PhysicsBullet(GameState * st);
		~PhysicsBullet();
		virtual void init();
		virtual void preGame();
		virtual void postGame();

	public:
		btRigidBody* addRigidBody(btCollisionShape* colShape, float mass, float x, float y, float z, CollisionGroup group);
		btRigidBody* addRigidBody(btCollisionShape* colShape, float m, btDefaultMotionState* motionState, CollisionGroup group);
		void addRigidBody(btRigidBody* body, CollisionGroup group);
		void addCollisionObject(btCollisionObject* obj, CollisionGroup group);
		void addAction(btActionInterface* action);
		void addVehicle(btRaycastVehicle* vehicle);
		void remRigidBody(btCollisionObject* body);
		void delRigidBody(btRigidBody* body);
		void delCollisionObject(btCollisionObject* body);
		void delAction(btActionInterface* action);

		void addCallback(PhysicsCallback *callback);
		PhysicsCallback* addCallback(PhysicsTickCallback func);
		PhysicsCallback* addCallback(PhysicsTickCallback func, Entity *e, void *data1, void *data2);
		void removeCallback(PhysicsCallback *callback);
		void handleCallback(float delta);

		void stepTime(float delta);
		void doCollisions();

		btDiscreteDynamicsWorld* getWorld();

		static void QuaternionToEulerXYZ(const btQuaternion &quat, btVector3 &euler);
		static float QuaternionToYaw(const btQuaternion &quat);
		btVector3 spawnLocation(float x, float z, float height);
};
