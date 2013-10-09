// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include "../rage.h"
#include "../gamestate.h"
#include "../render/animplay.h"
#include "../mod/vehicletype.h"
#include "../net/net_server.h"
#include "vehicle.h"


using namespace std;


float	wheelRadius = 0.5f;
float	wheelWidth = 0.4f;
float	wheelFriction = 1000;
float	suspensionStiffness = 20.f;
float	suspensionDamping = 2.3f;
float	suspensionCompression = 4.4f;
float	rollInfluence = 0.1f;
btVector3 wheelDirectionCS0(0,-1,0);
btVector3 wheelAxleCS(-1,0,0);



Vehicle::Vehicle(VehicleType *vt, GameState *st, float mapx, float mapy) : Entity(st)
{
	btVector3 sizeHE = vt->model->getBoundingSizeHE();
	
	btTransform trans = btTransform(
		btQuaternion(btScalar(0), btScalar(0), btScalar(0)),
		st->physics->spawnLocation(mapx, mapy, sizeHE.z() * 2.0f)
	);
	
	this->init(vt, st, trans);
}

Vehicle::Vehicle(VehicleType *vt, GameState *st, btTransform &loc) : Entity(st)
{
	this->init(vt, st, loc);
}

void Vehicle::init(VehicleType *vt, GameState *st, btTransform &loc)
{
	this->vt = vt;
	this->anim = new AnimPlay(vt->model);
	this->health = vt->health;

	this->engineForce = 0.0f;
	this->brakeForce = 0.0f;
	this->steering = 0.0f;

	btVector3 sizeHE = vt->model->getBoundingSizeHE();
	
	// LocalTrans effectively shifts the center of mass with respect to the chassis
	//btTransform localTrans;
	//localTrans.setIdentity();
	//localTrans.setOrigin(btVector3(0,0,0.5f));
	//compound->addChildShape(localTrans,vt->col_shape);
	
	btDefaultMotionState* motionState = new btDefaultMotionState(loc);
	this->body = st->physics->addRigidBody(vt->col_shape, 120.0f, motionState, CG_VEHICLE);
	this->body->setUserPointer(this);
	this->body->setActivationState(DISABLE_DEACTIVATION);
	
	// Create Vehicle
	this->vehicle_raycaster = new ChaoticRageVehicleRaycaster(st->physics->getWorld(), this);
	this->vehicle = new btRaycastVehicle(this->tuning, this->body, this->vehicle_raycaster);
	this->vehicle->setCoordinateSystem(0, 1, 2);
	st->physics->addVehicle(this->vehicle);
	
	// Create and attach wheels
	{
		btScalar suspensionRestLength(sizeHE.y() + 0.1f);

		this->wheel_shape = new btCylinderShapeX(btVector3(wheelWidth,wheelRadius,wheelRadius));
		
		btVector3 connectionPointCS0;
		float connectionHeight = 0.2f;
		
		
		connectionPointCS0 = btVector3(sizeHE.x(), connectionHeight, sizeHE.y());
		this->vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, this->tuning, false);
		
		connectionPointCS0 = btVector3(-sizeHE.x(), connectionHeight, sizeHE.y());
		this->vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, this->tuning, false);
		
		connectionPointCS0 = btVector3(sizeHE.x(), connectionHeight, -sizeHE.y());
		this->vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, this->tuning, false);
		
		connectionPointCS0 = btVector3(-sizeHE.x(), connectionHeight, -sizeHE.y());
		this->vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, this->tuning, false);
	}
	
	// Set some wheel dynamics
	for (int i = 0; i < this->vehicle->getNumWheels(); i++) {
		btWheelInfo& wheel = this->vehicle->getWheelInfo(i);
		
		wheel.m_suspensionStiffness = suspensionStiffness;
		wheel.m_wheelsDampingRelaxation = suspensionDamping;
		wheel.m_wheelsDampingCompression = suspensionCompression;
		wheel.m_frictionSlip = wheelFriction;
		wheel.m_rollInfluence = rollInfluence;
	}
}

Vehicle::~Vehicle()
{
	delete (this->anim);
	st->physics->delRigidBody(this->body);
}


float Vehicle::getSpeedKmHr()
{
	return this->vehicle->getCurrentSpeedKmHour();
}


/**
* Do stuff
**/
void Vehicle::update(int delta)
{
	int wheelIndex;

	if (this->health == 0) return;
	
	
	// Front
	wheelIndex = 0;
	this->vehicle->setSteeringValue(this->steering,wheelIndex);
	this->vehicle->setBrake(this->brakeForce,wheelIndex);
	
	wheelIndex = 1;
	this->vehicle->setSteeringValue(this->steering,wheelIndex);
	this->vehicle->setBrake(this->brakeForce,wheelIndex);
	
	
	// Rear
	wheelIndex = 2;
	this->vehicle->applyEngineForce(this->engineForce,wheelIndex);
	this->vehicle->setBrake(this->brakeForce,wheelIndex);
	
	wheelIndex = 3;
	this->vehicle->applyEngineForce(this->engineForce,wheelIndex);
	this->vehicle->setBrake(this->brakeForce,wheelIndex);
	
	
	for (int i = 0; i < this->vehicle->getNumWheels(); i++) {
		this->vehicle->updateWheelTransform(i, true);
	}

	// Send state over network
	// TODO: only when moving
	if (st->server != NULL) {
		st->server->addmsgVehicleState(this);
	}
}


/**
* Called by the unit to update driving status
**/
void Vehicle::operate(Unit* u, int key_up, int key_down, int key_left, int key_right, float horiz_angle, float vertical_angle)
{
	// Accel and brake
	if (key_up) {
		this->engineForce = MIN(this->engineForce + this->vt->engine_accel, this->vt->engine_max);
		this->brakeForce = 0.0f;
	} else if (key_down) {
		if (this->getSpeedKmHr() > 0.0) {
			this->brakeForce = MIN(this->brakeForce + this->vt->brake_accel, this->vt->brake_max);
			this->engineForce = 0.0f;
		} else {
			this->engineForce = MAX(this->engineForce - this->vt->reverse_accel, 0.0f - this->vt->reverse_max);
			this->brakeForce = 0.0f;
		}
	} else {
		this->engineForce = MAX(this->engineForce - 20.0f, 0.0f);			// Dampening
		this->brakeForce = MAX(this->brakeForce - 15.0f, 0.0f);
	}

	// Steering
	if (key_left) {
		this->steering = MIN(this->steering + 0.01f, 0.3f);
	} else if (key_right) {
		this->steering = MAX(this->steering - 0.01f, -0.3f);
	} else if (this->steering > 0.0f) {
		this->steering = MAX(this->steering - 0.01f, 0.0f);
	} else if  (this->steering < 0.0f) {
		this->steering = MIN(this->steering + 0.01f, 0.0f);
	}

	// Turret
	if (this->vt->move_node != "") {
		glm::mat4 turret = glm::toMat4(glm::rotate(glm::quat(), vertical_angle, this->vt->move_axis));
		this->getAnimModel()->setMoveTransform(this->vt->move_node, turret);
	}
}


AnimPlay* Vehicle::getAnimModel()
{
	return this->anim;
}

Sound* Vehicle::getSound()
{
	return NULL;
}


/**
* We have been hit! Take some damage
**/
void Vehicle::takeDamage(int damage)
{
	this->health -= damage;
	if (this->health < 0) this->health = 0;
	
	for (unsigned int j = 0; j < this->vt->damage_models.size(); j++) {
		VehicleTypeDamage * dam = this->vt->damage_models.at(j);
		
		if (this->health <= dam->health) {
			delete(this->anim);
			this->anim = new AnimPlay(dam->model);
			break;
		}
	}
}


/**
* Handle raycasting for vehicle wheels.
* We detect both terrain and water, but return a "miss" if it hits water.
**/
void* ChaoticRageVehicleRaycaster::castRay(const btVector3& from,const btVector3& to, btVehicleRaycasterResult& result)
{
	btCollisionWorld::ClosestRayResultCallback rayCallback(from,to);
	rayCallback.m_collisionFilterGroup = CG_VEHICLE;
	rayCallback.m_collisionFilterMask = CG_TERRAIN | CG_WATER;

	m_dynamicsWorld->rayTest(from, to, rayCallback);

	if (rayCallback.hasHit())
	{
		if (!this->v->vt->land && rayCallback.m_collisionObject->getBroadphaseHandle()->m_collisionFilterGroup == CG_TERRAIN) {
			return 0;
		}

		if (!this->v->vt->water && rayCallback.m_collisionObject->getBroadphaseHandle()->m_collisionFilterGroup == CG_WATER) {
			return 0;
		}

		const btRigidBody* body = btRigidBody::upcast(rayCallback.m_collisionObject);
        if (body && body->hasContactResponse())
		{
			result.m_hitPointInWorld = rayCallback.m_hitPointWorld;
			result.m_hitNormalInWorld = rayCallback.m_hitNormalWorld;
			result.m_hitNormalInWorld.normalize();
			result.m_distFraction = rayCallback.m_closestHitFraction;
			return (void*)body;
		}
	}

	return 0;
}

