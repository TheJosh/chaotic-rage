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


Helicopter::Helicopter(VehicleType *vt, GameState *st, float mapx, float mapy) : Vehicle(st)
{
	btVector3 sizeHE = vt->model->getBoundingSizeHE();
	
	btTransform trans = btTransform(
		btQuaternion(btScalar(0), btScalar(0), btScalar(0)),
		st->physics->spawnLocation(mapx, mapy, sizeHE.z() * 2.0f)
	);
	
	this->init(vt, st, trans);
}

Helicopter::Helicopter(VehicleType *vt, GameState *st, btTransform &loc) : Vehicle(st)
{
	this->init(vt, st, loc);
}


/**
* Init all the physics stuff
**/
void Helicopter::init(VehicleType *vt, GameState *st, btTransform &loc)
{
	this->vt = vt;
	this->anim = new AnimPlay(vt->model);
	this->health = vt->health;

	btVector3 sizeHE = vt->model->getBoundingSizeHE();
	
	btDefaultMotionState* motionState = new btDefaultMotionState(loc);
	this->body = st->physics->addRigidBody(vt->col_shape, 120.0f, motionState, CG_VEHICLE);
	this->body->setUserPointer(this);
	this->body->setActivationState(DISABLE_DEACTIVATION);
	
	this->running = false;
}


/**
* Clean up physics stuff
**/
Helicopter::~Helicopter()
{
}


/**
* Update the vehicle
**/
void Helicopter::update(int delta)
{
	if (! this->running) return;
	if (this->health == 0) return;
	
	// Steering
	btQuaternion rot = btQuaternion(this->steering, 0.0f, 0.0f);
	btMatrix3x3& matRot = btMatrix3x3(rot);
	this->body->getWorldTransform().setBasis(matRot);

	// Lift force
	btVector3 liftForce = btVector3(0.0f, this->lift * delta, 0.0f);

	// Forward force
	btVector3 relativeForce = btVector3(0.0f, 0.0f, this->forward * delta);
	btVector3 forwardForce = matRot * relativeForce;

	btVector3 force = liftForce + forwardForce;
	this->body->activate(true);
	this->body->applyCentralImpulse(force);
}


void Helicopter::enter()
{
	this->running = true;
	this->lift = 0.0f;
	this->forward = 0.0f;
}


void Helicopter::exit()
{
	this->running = false;
}


/**
* Called by the unit to update driving status
**/
void Helicopter::operate(Unit* u, int key_up, int key_down, int key_left, int key_right, float horiz_angle, float vert_angle)
{
	if (key_up) {
		this->forward += 0.15f;
		if (this->forward > 5.0f) this->forward = 5.0f;
	} else {
		this->forward = 0.0f;
	}

	if (key_down) {
		this->lift += 0.15f;
		if (this->lift > 5.0f) this->lift = 5.0f;
	} else {
		this->lift = 0.0f;
	}

	this->steering = horiz_angle / 20.0f;
}


