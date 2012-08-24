// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"

using namespace std;


#define CUBE_HALF_EXTENTS 1

float	gEngineForce = 0.f;
float	gBreakingForce = 0.f;

float	maxEngineForce = 1000.f;//this should be engine/velocity dependent
float	maxBreakingForce = 100.f;

float	gVehicleSteering = 0.f;
float	steeringIncrement = 0.04f;
float	steeringClamp = 0.3f;
float	wheelRadius = 0.5f;
float	wheelWidth = 0.4f;
float	wheelFriction = 1000;//BT_LARGE_FLOAT;
float	suspensionStiffness = 20.f;
float	suspensionDamping = 2.3f;
float	suspensionCompression = 4.4f;
float	rollInfluence = 0.1f;//1.0f;

btScalar suspensionRestLength(0.6f);

btVector3 wheelDirectionCS0(0,0,-1);
btVector3 wheelAxleCS(1,0,0);



Vehicle::Vehicle(VehicleType *vt, GameState *st, float x, float y, float z, float angle) : Entity(st)
{
	this->vt = vt;
	this->anim = new AnimPlay(vt->model);
	this->health = vt->health;


	// TODO: The colShape should be tied to the object type.
	// TODO: Store the colshape and nuke at some point
	btCollisionShape* chassisShape = new btBoxShape(btVector3(1.f, 2.f, 0.5f));
	//btCompoundShape* compound = new btCompoundShape();
	
	// LocalTrans effectively shifts the center of mass with respect to the chassis
	//btTransform localTrans;
	//localTrans.setIdentity();
	//localTrans.setOrigin(btVector3(0,0,0.5f));
	//compound->addChildShape(localTrans,chassisShape);
	
	btDefaultMotionState* motionState =
		new btDefaultMotionState(btTransform(
			btQuaternion(btScalar(0), btScalar(0), btScalar(0)),
			btVector3(x,y,1.5f)
		));
	this->body = st->physics->addRigidBody(chassisShape, 30.0f, motionState);
	
	this->body->setUserPointer(this);
	//this->body->setLinearVelocity(btVector3(0,0,0));
	//this->body->setAngularVelocity(btVector3(0,0,0));
	this->body->setActivationState(DISABLE_DEACTIVATION);
	
	//this->tuning.m_maxSuspensionTravelCm = 500.0f;
	//this->tuning.m_suspensionCompression = 4.4f;
	//this->tuning.m_suspensionDamping = 2.3f;
	//this->tuning.m_frictionSlip = 1000.0f;
	//this->tuning.m_suspensionStiffness = 20.0f;

	
	// Create Vehicle
	this->vehicle_raycaster = new btDefaultVehicleRaycaster(st->physics->getWorld());
	this->vehicle = new btRaycastVehicle(this->tuning, this->body, this->vehicle_raycaster);
	
	st->physics->getWorld()->addAction(this->vehicle);
	
	
	// Create and attach wheels
	
	this->vehicle->setCoordinateSystem(0, 2, 1);
	
	{
		this->wheel_shape = new btCylinderShapeX(btVector3(wheelWidth,wheelRadius,wheelRadius));
		
		btVector3 connectionPointCS0;
		
		float connectionHeight = 0.2f;
		bool isFrontWheel = true;
		
		
		connectionPointCS0 = btVector3(0.88f, 1.6f, connectionHeight);
		this->vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, this->tuning, isFrontWheel);
		
		connectionPointCS0 = btVector3(-0.88f, 1.6f, connectionHeight);
		this->vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, this->tuning, isFrontWheel);
		
		isFrontWheel = false;
		
		connectionPointCS0 = btVector3(0.88f, -1.6f, connectionHeight);
		this->vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, this->tuning, isFrontWheel);
		
		connectionPointCS0 = btVector3(-0.88f, -1.6f, connectionHeight);
		this->vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, this->tuning, isFrontWheel);
	}
	
	// Set some wheel dynamics
	/*for (int i = 0; i < this->vehicle->getNumWheels(); i++) {
		btWheelInfo& wheel = this->vehicle->getWheelInfo(i);
		
		wheel.m_suspensionStiffness = suspensionStiffness;
		wheel.m_wheelsDampingRelaxation = suspensionDamping;
		wheel.m_wheelsDampingCompression = suspensionCompression;
		wheel.m_frictionSlip = wheelFriction;
		wheel.m_rollInfluence = rollInfluence;
	}*/
	
}

Vehicle::~Vehicle()
{
	delete (this->anim);
	st->physics->delRigidBody(this->body);
}


void Vehicle::hasBeenHit(Entity * that)
{
}


/**
* Do stuff
**/
void Vehicle::update(int delta)
{
	//if (this->anim->isDone()) this->anim->next();
	//if (this->health == 0) return;
	
	//return;
	
	
	int wheelIndex;
	
	
	// Rear
	wheelIndex = 2;
	this->vehicle->applyEngineForce(gEngineForce,wheelIndex);
	this->vehicle->setBrake(gBreakingForce,wheelIndex);
	
	wheelIndex = 3;
	this->vehicle->applyEngineForce(gEngineForce,wheelIndex);
	this->vehicle->setBrake(gBreakingForce,wheelIndex);
	
	// Front
	wheelIndex = 0;
	this->vehicle->setSteeringValue(gVehicleSteering,wheelIndex);
	
	wheelIndex = 1;
	this->vehicle->setSteeringValue(gVehicleSteering,wheelIndex);
	
	
	for (int i = 0; i < this->vehicle->getNumWheels(); i++) {
		this->vehicle->updateWheelTransform(i, true);
	}
	
	gEngineForce += 1.0f;
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

