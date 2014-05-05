// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include "../rage.h"
#include "../physics_bullet.h"
#include "../game_state.h"
#include "../render_opengl/animplay.h"
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

	vector <VehicleTypeNode>::iterator it;
	for (it = this->vt->nodes.begin(); it != this->vt->nodes.end(); ++it) {
		this->anim->addMoveNode((*it).node);
	}

	btDefaultMotionState* motionState = new btDefaultMotionState(loc);
	this->body = st->physics->addRigidBody(vt->col_shape, vt->mass, motionState, CG_VEHICLE);
	this->body->setUserPointer(this);
	
	this->running = false;
}


/**
* Clean up physics stuff
**/
Helicopter::~Helicopter()
{
	delete(this->anim);
	this->anim = NULL;

	st->physics->delRigidBody(this->body);
	this->body = NULL;
}


/**
* Update the vehicle
**/
void Helicopter::update(int delta)
{
	if (! this->running) return;
	if (this->health == 0) return;
	
	this->body->activate(true);
	
	// Steering + tilt
	btQuaternion rot = btQuaternion(this->yaw, this->pitch, this->roll);
	btMatrix3x3 rotMat = btMatrix3x3(rot);
	this->body->getWorldTransform().setBasis(rotMat);
	
	// Forward + lift force
	btVector3 relativeForce = btVector3(0.0f, this->lift * delta, this->forward * delta);
	btVector3 absForce = rotMat * relativeForce;
	this->body->applyCentralImpulse(absForce);
	
	// Blades animation
	int frame = this->st->anim_frame % 12;
	this->setNodeAngle(VEHICLE_NODE_SPIN, 360.0f / 12.0f * frame);
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
	float currY = this->getTransform().getOrigin().y();
	
	if (currY > 50.0f) {
		this->lift = -0.01f * delta;
	} else if (key_down) {
		this->lift = MIN(this->lift + 0.15f * delta, 2.0f);
	} else {
		this->lift = 0.0f;
	}
	
	this->yaw = DEG_TO_RAD(horiz_angle);
	
	if (currY > 5.0f) {
		if (key_up) {
			this->forward = MIN(this->forward + 0.15f * delta, 3.0f);
			this->pitch = MIN(this->pitch + 0.002f * delta, PI / 8.0f);
		} else {
			this->forward = 0.0f;
			this->pitch = MAX(this->pitch - 0.01f * delta, 0.0f);
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
	}
}


