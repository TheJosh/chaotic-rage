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

btScalar suspensionRestLength(0.6);

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
	btCompoundShape* compound = new btCompoundShape();
	
	// LocalTrans effectively shifts the center of mass with respect to the chassis
	btTransform localTrans;
	localTrans.setIdentity();
	localTrans.setOrigin(btVector3(0,0,1));
	compound->addChildShape(localTrans,chassisShape);
	
	btDefaultMotionState* motionState =
		new btDefaultMotionState(btTransform(
			btQuaternion(btScalar(0), btScalar(0), btScalar(0)),
			btVector3(x,y,1.0f)
		));	
	this->body = st->physics->addRigidBody(compound, 10.0, motionState);
	this->body->setUserPointer(this);
	
	
	        gVehicleSteering = 0.f;
        //this->body->setCenterOfMassTransform(btTransform::getIdentity());
        //this->body->setLinearVelocity(btVector3(0,0,0));
        //this->body->setAngularVelocity(btVector3(0,0,0));
        //st->physics->getWorld()->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(this->body->getBroadphaseHandle(),st->physics->getWorld()->getDispatcher());
			


	// Create wheel
	this->wheel_shape = new btCylinderShapeX(btVector3(wheelWidth,wheelRadius,wheelRadius));
	
	this->vehicle_raycaster = new btDefaultVehicleRaycaster(st->physics->getWorld());
	this->vehicle = new btRaycastVehicle(this->tuning, this->body, this->vehicle_raycaster);
	
	this->body->setActivationState(DISABLE_DEACTIVATION);
	
	st->physics->addVehicle(this->vehicle);
	
	this->vehicle->setCoordinateSystem(0, 2, 1);
	
	{
		btVector3 connectionPointCS0;
		
		float connectionHeight = 1.2f;
		bool isFrontWheel = true;
		
		
		connectionPointCS0 = btVector3(CUBE_HALF_EXTENTS-(0.3*wheelWidth),2*CUBE_HALF_EXTENTS-wheelRadius, connectionHeight);
		this->vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, this->tuning, isFrontWheel);
		
		connectionPointCS0 = btVector3(-CUBE_HALF_EXTENTS+(0.3*wheelWidth),2*CUBE_HALF_EXTENTS-wheelRadius, connectionHeight);
		this->vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, this->tuning, isFrontWheel);
		
		isFrontWheel = false;
		
		connectionPointCS0 = btVector3(-CUBE_HALF_EXTENTS+(0.3*wheelWidth),-2*CUBE_HALF_EXTENTS+wheelRadius, connectionHeight);
		this->vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, this->tuning, isFrontWheel);
		
		connectionPointCS0 = btVector3(CUBE_HALF_EXTENTS-(0.3*wheelWidth),-2*CUBE_HALF_EXTENTS+wheelRadius, connectionHeight);
		this->vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, this->tuning, isFrontWheel);
	}
	
	this->vehicle->resetSuspension();

	for (int i = 0; i < this->vehicle->getNumWheels(); i++) {
		btWheelInfo& wheel = this->vehicle->getWheelInfo(i);
		
		wheel.m_suspensionStiffness = suspensionStiffness;
		wheel.m_wheelsDampingRelaxation = suspensionDamping;
		wheel.m_wheelsDampingCompression = suspensionCompression;
		wheel.m_frictionSlip = wheelFriction;
		wheel.m_rollInfluence = rollInfluence;
		
		this->vehicle->updateWheelTransform(i, true);
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


/**
* Do stuff
**/
void Vehicle::update(int delta)
{
	//if (this->anim->isDone()) this->anim->next();
	//if (this->health == 0) return;
	
	int wheelIndex = 0;
	float gEngineForce = 100.0f;
	float gBreakingForce = 0.0f;
	float gVehicleSteering = 0.0f;
	
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

