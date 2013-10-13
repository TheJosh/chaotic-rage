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
#include "helicopter.h"


using namespace std;


static float	wheelRadius = 0.5f;
static float	wheelWidth = 0.4f;
static float	wheelFriction = 1000;
static float	suspensionStiffness = 20.f;
static float	suspensionDamping = 2.3f;
static float	suspensionCompression = 4.4f;
static float	rollInfluence = 0.1f;
static btVector3 wheelDirectionCS0(0,-1,0);
static btVector3 wheelAxleCS(-1,0,0);



void Helicopter::init(VehicleType *vt, GameState *st, btTransform &loc)
{
	this->vt = vt;
	this->anim = new AnimPlay(vt->model);
	this->health = vt->health;

	if (vt->vert_move_node != "") {
		this->anim->addMoveNode(vt->vert_move_node);
	}
	
	if (vt->horiz_move_node != "") {
		this->anim->addMoveNode(vt->horiz_move_node);
	}
	
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
	// TODO: Optimize this for fixed turrets
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


float Helicopter::getSpeedKmHr()
{
	return this->vehicle->getCurrentSpeedKmHour();
}


/**
* Do stuff
**/
void Helicopter::update(int delta)
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
void Helicopter::operate(Unit* u, int key_up, int key_down, int key_left, int key_right, float horiz_angle, float vert_angle)
{
	// Accel and brake
	if (this->vt->engine) {
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
	}
	
	// Steering
	if (this->vt->steer) {
		if (key_left) {
			this->steering = MIN(this->steering + 0.01f, 0.3f);
		} else if (key_right) {
			this->steering = MAX(this->steering - 0.01f, -0.3f);
		} else if (this->steering > 0.0f) {
			this->steering = MAX(this->steering - 0.01f, 0.0f);
		} else if  (this->steering < 0.0f) {
			this->steering = MIN(this->steering + 0.01f, 0.0f);
		}
	}

	// Horizontal move node
	if (this->vt->horiz_move_node != "") {
		glm::mat4 rotation = glm::toMat4(glm::rotate(glm::quat(), horiz_angle, this->vt->horiz_move_axis));
		this->getAnimModel()->setMoveTransform(this->vt->horiz_move_node, rotation);
	}

	// Vertical move node
	if (this->vt->vert_move_node != "") {
		glm::mat4 rotation = glm::toMat4(glm::rotate(glm::quat(), vert_angle, this->vt->vert_move_axis));
		this->getAnimModel()->setMoveTransform(this->vt->vert_move_node, rotation);
	}
}