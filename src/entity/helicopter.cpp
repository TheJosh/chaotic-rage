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

	if (vt->horiz_move_node != "") {
		this->anim->addMoveNode(vt->horiz_move_node);
	}

	btVector3 sizeHE = vt->model->getBoundingSizeHE();
	
	btDefaultMotionState* motionState = new btDefaultMotionState(loc);
	this->body = st->physics->addRigidBody(vt->col_shape, vt->mass, motionState, CG_VEHICLE);
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
	
	// Steering + tilt
	btQuaternion rot = btQuaternion(this->yaw, this->pitch, this->roll);
	btMatrix3x3 matRot = btMatrix3x3(rot);
	this->body->getWorldTransform().setBasis(matRot);

	// Forward + lift force
	btVector3 relativeForce = btVector3(0.0f, this->lift * delta, this->forward * delta);
	btVector3 absForce = matRot * relativeForce;

	this->body->activate(true);
	this->body->applyCentralImpulse(absForce);

	// Blades
	if (this->vt->horiz_move_node != "") {
		int frame = this->st->anim_frame % 12;
		glm::mat4 rotation = glm::toMat4(glm::rotate(glm::quat(), 360/12*frame, this->vt->horiz_move_axis));
		this->getAnimModel()->setMoveTransform(this->vt->horiz_move_node, rotation);
	}
}


/**
* Return the weapon transform for this vehicle
**/
void Helicopter::getWeaponTransform(btTransform &xform)
{
	xform = this->getTransform();
	
	btMatrix3x3 down = xform.getBasis() * btMatrix3x3(btQuaternion(0.0f, PI / 2.5f, 0.0f));
	xform.setBasis(down);
}


void Helicopter::enter()
{
	this->running = true;
	this->lift = 0.0f;
	this->forward = 0.0f;
	this->yaw = 0.0f;
	this->pitch = 0.0f;
	this->roll = 0.0f;
}


void Helicopter::exit()
{
	this->running = false;
}


/**
* Called by the unit to update driving status
**/
void Helicopter::operate(Unit* u, int delta, int key_up, int key_down, int key_left, int key_right, float horiz_angle, float vert_angle)
{
	if (key_up) {
		this->forward = MIN(this->forward + 0.15f, 5.0f);
		this->pitch = MIN(this->pitch + 0.002f * delta, PI / 8.0f);
		
	} else {
		this->forward = 0.0f;
		this->pitch = MAX(this->pitch - 0.01f, 0.0f);
	}

	if (key_down) {
		this->lift = MIN(this->lift + 0.15f, 3.0f);
	} else {
		this->lift = 0.0f;
	}

	if (key_right) {
		this->roll = MIN(this->roll + 0.002f * delta, PI / 6.0f);
	} else if (key_left) {
		this->roll = MAX(this->roll - 0.002f * delta, PI / -6.0f);
	} else if (this->roll > 0.0f) {
		this->roll = MAX(this->roll - 0.002f * delta, 0.0f);
	} else if (this->roll < 0.0f) {
		this->roll = MIN(this->roll + 0.002f * delta, 0.0f);
	}
	
	this->yaw = DEG_TO_RAD(horiz_angle);
	//this->pitch = vert_angle;
}


