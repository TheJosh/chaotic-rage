// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"

using namespace std;


float	maxEngineForce = 1000.f;//this should be engine/velocity dependent
float	maxBreakingForce = 100.f;
float	steeringIncrement = 0.04f;
float	steeringClamp = 0.3f;
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
	
	// TODO: The colShape should be tied to the object type.
	// TODO: Store the colshape and nuke at some point
	btCollisionShape* chassisShape = new btBoxShape(sizeHE);
	//btCompoundShape* compound = new btCompoundShape();
	
	// LocalTrans effectively shifts the center of mass with respect to the chassis
	//btTransform localTrans;
	//localTrans.setIdentity();
	//localTrans.setOrigin(btVector3(0,0,0.5f));
	//compound->addChildShape(localTrans,chassisShape);
	
	btDefaultMotionState* motionState = new btDefaultMotionState(loc);
	this->body = st->physics->addRigidBody(chassisShape, 120.0f, motionState, CollisionGroup::CG_VEHICLE);
	this->body->setUserPointer(this);
	this->body->setActivationState(DISABLE_DEACTIVATION);
	
	// Create Vehicle
	this->vehicle_raycaster = new btDefaultVehicleRaycaster(st->physics->getWorld());
	this->vehicle = new btRaycastVehicle(this->tuning, this->body, this->vehicle_raycaster);
	this->vehicle->setCoordinateSystem(0, 1, 2);
	st->physics->addVehicle(this->vehicle);
	
	// Create and attach wheels
	{
		btScalar suspensionRestLength(sizeHE.y() + 0.1f);

		this->wheel_shape = new btCylinderShapeX(btVector3(wheelWidth,wheelRadius,wheelRadius));
		
		btVector3 connectionPointCS0;
		
		float connectionHeight = 0.2f;
		bool isFrontWheel = true;
		
		
		connectionPointCS0 = btVector3(sizeHE.x(), connectionHeight, sizeHE.y());
		this->vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, this->tuning, isFrontWheel);
		
		connectionPointCS0 = btVector3(-sizeHE.x(), connectionHeight, sizeHE.y());
		this->vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, this->tuning, isFrontWheel);
		
		isFrontWheel = false;
		
		connectionPointCS0 = btVector3(sizeHE.x(), connectionHeight, -sizeHE.y());
		this->vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, this->tuning, isFrontWheel);
		
		connectionPointCS0 = btVector3(-sizeHE.x(), connectionHeight, -sizeHE.y());
		this->vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, this->tuning, isFrontWheel);
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


void Vehicle::hasBeenHit(Entity * that)
{
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

	if (this->anim->isDone()) this->anim->next();
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

