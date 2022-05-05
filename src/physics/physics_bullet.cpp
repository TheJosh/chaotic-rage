// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <map>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include "physics_bullet.h"
#include "physics_callback.h"
#include "../game_state.h"
#include "../entity/entity.h"

using namespace std;


static void bulletHandleCallback(btDynamicsWorld *world, btScalar timeStep);


PhysicsBullet::PhysicsBullet(GameState * st)
{
	this->st = st;
	st->physics = this;
	this->collisionConfiguration = NULL;
	this->dispatcher = NULL;
	this->broadphase = NULL;
	this->solver = NULL;
	this->dynamicsWorld = NULL;
	this->collisionShapes = NULL;
	this->ghostPairCallback = NULL;
	this->groundRigidBody = NULL;

	int moststuff = CG_TERRAIN | CG_AMMO | CG_WALL | CG_OBJECT | CG_UNIT | CG_VEHICLE | CG_PICKUP;

	this->masks[CG_NOTHING] = 0;
	this->masks[CG_TERRAIN] = CG_DEBRIS | CG_WALL | CG_OBJECT | CG_UNIT | CG_VEHICLE;
	this->masks[CG_WATER] = CG_VEHICLE;
	this->masks[CG_DEBRIS] = CG_TERRAIN;
	this->masks[CG_AMMO] = CG_WALL | CG_OBJECT | CG_UNIT | CG_VEHICLE;
	this->masks[CG_WALL] = moststuff;
	this->masks[CG_OBJECT] = moststuff;
	this->masks[CG_UNIT] = moststuff;
	this->masks[CG_VEHICLE] = moststuff;
	this->masks[CG_PICKUP] = moststuff;
}

PhysicsBullet::~PhysicsBullet()
{
}


/**
* This is run before map load to set up stuff
**/
void PhysicsBullet::init()
{
	broadphase = new btDbvtBroadphase();

	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);

	ghostPairCallback = new btGhostPairCallback();
	broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(ghostPairCallback);

	solver = new btSequentialImpulseConstraintSolver();

	dynamicsWorld = new btDiscreteDynamicsWorld(
		dispatcher,
		broadphase,
		solver,
		collisionConfiguration
	);

	btContactSolverInfo& info = dynamicsWorld->getSolverInfo();
	info.m_solverMode |= SOLVER_ENABLE_FRICTION_DIRECTION_CACHING;

	dynamicsWorld->setGravity(btVector3(0,-10,0));
	dynamicsWorld->setInternalTickCallback(bulletHandleCallback, static_cast<void *>(this));
}


/**
* This is post-map load, pre game run
**/
void PhysicsBullet::preGame()
{
	collisionShapes = new btAlignedObjectArray<btCollisionShape*>();
}


/**
* Tear down the physics world
**/
void PhysicsBullet::postGame()
{
	// TODO: Free other stuff properly, see HelloWorld demo

	delete collisionShapes;

	//delete dynamicsWorld;
	delete solver;
	delete broadphase;
	delete ghostPairCallback;
	delete dispatcher;
	delete collisionConfiguration;

	collisionShapes = NULL;
	ghostPairCallback = NULL;
	dynamicsWorld = NULL;
	solver = NULL;
	broadphase = NULL;
	dispatcher = NULL;
	collisionConfiguration = NULL;
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
* colShape = The collission shape of the body
* m = mass
* x,y,z = origin position
**/
btRigidBody* PhysicsBullet::addRigidBody(btCollisionShape* colShape, float m, float x, float y, float z, CollisionGroup group)
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

	dynamicsWorld->addRigidBody(body, group, this->masks[group]);

	body->setFriction(0.8f);

	return body;
}


/**
* Create and add a rigid body
*
* colShape = The collission shape of the body
* m = mass
* motionState = origin motion state (position and rotation)
**/
btRigidBody* PhysicsBullet::addRigidBody(btCollisionShape* colShape, float m, btDefaultMotionState* motionState, CollisionGroup group)
{
	btScalar mass(m);
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0,0,0);
	if (isDynamic)
		colShape->calculateLocalInertia(mass,localInertia);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(
		mass,
		motionState,
		colShape,
		localInertia
	);
	btRigidBody* body = new btRigidBody(rbInfo);

	dynamicsWorld->addRigidBody(body, group, this->masks[group]);

	body->setFriction(0.8f);

	return body;
}


/**
* Add a rigid body which already exists
**/
void PhysicsBullet::addRigidBody(btRigidBody* body, CollisionGroup group)
{
	dynamicsWorld->addRigidBody(body, group, this->masks[group]);
}


/**
* Add a collision object
**/
void PhysicsBullet::addCollisionObject(btCollisionObject* obj, CollisionGroup group)
{
	dynamicsWorld->addCollisionObject(obj, group, this->masks[group]);
}


/**
* Add an action
**/
void PhysicsBullet::addAction(btActionInterface* action)
{
	dynamicsWorld->addAction(action);
}


/**
* Add a vehicle
**/
void PhysicsBullet::addVehicle(btRaycastVehicle* vehicle)
{
	dynamicsWorld->addAction(vehicle);
}


/**
* Remove a btCollisionObject from the game world, but keep the object around for adding later.
**/
void PhysicsBullet::remRigidBody(btCollisionObject* body)
{
	dynamicsWorld->removeCollisionObject(body);
}


/**
* Delete a rigid body, and remove it from the game world too
**/
void PhysicsBullet::delRigidBody(btRigidBody* body)
{
	if (!body) return;

	if (body->getMotionState()) {
		delete body->getMotionState();
	}

	dynamicsWorld->removeCollisionObject(body);

	delete body;
}


/**
* Delete a btCollisionObject, and remove it from the game world too
**/
void PhysicsBullet::delCollisionObject(btCollisionObject* body)
{
	dynamicsWorld->removeCollisionObject(body);
	delete body;
}


/**
* Delete an action
**/
void PhysicsBullet::delAction(btActionInterface* action)
{
	dynamicsWorld->removeAction(action);
	delete action;
}


/**
* Add a callback
**/
void PhysicsBullet::addCallback(PhysicsCallback *callback)
{
	this->callbacks.push_back(callback);
}


/**
* Add a callback
**/
PhysicsCallback* PhysicsBullet::addCallback(PhysicsTickCallback func)
{
	return this->addCallback(func, NULL, NULL, NULL);
}


/**
* Add a callback
**/
PhysicsCallback* PhysicsBullet::addCallback(PhysicsTickCallback func, Entity *e, void *data1, void *data2)
{
	PhysicsCallback *callback = new PhysicsCallback();
	callback->func = func;
	callback->e = e;
	callback->data1 = data1;
	callback->data2 = data2;
	this->addCallback(callback);
	return callback;
}


/**
* Remove a callback
* Only actually marks for removal, the actual removal is in the handleCallback() method
**/
void PhysicsBullet::removeCallback(PhysicsCallback *callback)
{
	callback->func = NULL;
}


/**
* Glue function between bullet and our class
**/
void bulletHandleCallback(btDynamicsWorld *world, btScalar timeStep)
{
	PhysicsBullet *p = static_cast<PhysicsBullet *>(world->getWorldUserInfo());
	p->handleCallback(timeStep);
}


/**
* Predicate for remove_if in handleCallback
**/
bool is_removed_callback(const PhysicsCallback* value) { return (value->func == NULL); }


/**
* Called by bullet to handle the callbacks
**/
void PhysicsBullet::handleCallback(float delta)
{
	this->callbacks.remove_if(is_removed_callback);

	for (std::list<PhysicsCallback*>::iterator it = this->callbacks.begin(); it != this->callbacks.end(); ++it) {
		PhysicsCallback *callback = (*it);
		(*callback->func)(delta, callback->e, callback->data1, callback->data2);
	}

	float deltaMs = delta * 1000.0f;
	for (std::list<Entity*>::iterator it = st->entities.begin(); it != st->entities.end(); ++it) {
		Entity* e = (*it);
		e->physicsUpdate(deltaMs);
	}
}


/**
* Step the physics forward by the given amount of time
*
* Very useful docs:
*   http://bulletphysics.org/mediawiki-1.5.8/index.php/Stepping_The_World
*
* Currently running at 60fps with up to 3 substeps
* Which means below 20fps the simulation will "lose" time
**/
void PhysicsBullet::stepTime(float delta)
{
	dynamicsWorld->stepSimulation(
		delta / 1000.0f,
		3,
		1.0f/60.0f
	);
}


/**
* Use a raytest to find an appropriate spawn location on the ground
**/
btVector3 PhysicsBullet::spawnLocation(float x, float z, float height)
{
	float y = 1000.0f;

	btVector3 begin(x, y, z);
	btVector3 end(x, -y, z);

	btCollisionWorld::ClosestRayResultCallback cb(begin, end);
	cb.m_collisionFilterGroup = CG_WALL;
	cb.m_collisionFilterMask = CG_TERRAIN;

	dynamicsWorld->rayTest(begin, end, cb);

	if (cb.hasHit()) {
		y = cb.m_hitPointWorld.y() + height/2.0f;
	}

	return btVector3(x, y, z);
}


/**
* Convert a quaternion into euler angles
**/
void PhysicsBullet::QuaternionToEulerXYZ(const btQuaternion &quat, btVector3 &euler)
{
	float w=quat.getW(); float x=quat.getX(); float y=quat.getY(); float z=quat.getZ();
	double sqw = static_cast<double>(w*w);
	double sqx = static_cast<double>(x*x);
	double sqy = static_cast<double>(y*y);
	double sqz = static_cast<double>(z*z);

	euler.setZ(static_cast<float>(atan2(2.0 * (x*y + z*w),(sqx - sqy - sqz + sqw))));
	euler.setX(static_cast<float>(atan2(2.0 * (y*z + x*w),(-sqx - sqy + sqz + sqw))));
	euler.setY(static_cast<float>(asin(-2.0 * (x*z - y*w))));
}


/**
* Get the yaw from a quaternion
**/
float PhysicsBullet::QuaternionToYaw(const btQuaternion &quat)
{
	float fTx  = 2.0f*quat.x();
	float fTy  = 2.0f*quat.y();
	float fTz  = 2.0f*quat.z();
	float fTwy = fTy*quat.w();
	float fTxx = fTx*quat.x();
	float fTxz = fTz*quat.x();
	float fTyy = fTy*quat.y();

	return atan2(fTxz+fTwy, 1.0f-(fTxx+fTyy));
}
