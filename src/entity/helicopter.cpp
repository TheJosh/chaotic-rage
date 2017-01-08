// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "helicopter.h"
#include <vector>
#include "../game_engine.h"
#include "../game_state.h"
#include "../mod/vehicletype.h"
#include "../net/net_server.h"
#include "../physics/physics_bullet.h"
#include "../rage.h"
#include "../render_opengl/animplay.h"
#include "entity.h"
#include "vehicle.h"
#include "unit/player.h"

class Unit;
class btTransform;

using namespace std;


/**
* Create helicopter at Map X/Z coordinates (with Y calculated automatically)
**/
Helicopter::Helicopter(VehicleType *vt, GameState *st, float x, float z) : Vehicle(st)
{
	btVector3 size = vt->model->getBoundingSize();

	btTransform trans = btTransform(
		btQuaternion(btScalar(0), btScalar(0), btScalar(0)),
		st->physics->spawnLocation(x, z, size.z())
	);

	this->init(vt, st, trans);
}


/**
* Create helicopter at X/Y/Z coordinates
**/
Helicopter::Helicopter(VehicleType *vt, GameState *st, float x, float y, float z) : Vehicle(st)
{
	btTransform trans = btTransform(
		btQuaternion(btScalar(0), btScalar(0), btScalar(0)),
		btVector3(x, y, z)
	);

	this->init(vt, st, trans);
}


/**
* Create helicopter at location specified by a btTransform
**/
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
	st->addAnimPlay(this->anim, this);

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
	st->remAnimPlay(this->anim);
	delete(this->anim);
	this->anim = NULL;

	st->physics->delRigidBody(this->body);
	this->body = NULL;
}


/**
* Update the vehicle
**/
void Helicopter::update(float delta)
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
	int angle = (this->st->anim_frame * 75) % 360;
	this->setNodeAngle(VEHICLE_NODE_SPIN, static_cast<float>(angle));
	
	// Send state over network
	if (GEng()->server != NULL) {
		GEng()->server->addmsgVehicleState(this);
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


/**
* Called when a unit enters the vehicle
**/
void Helicopter::enter()
{
	this->running = true;
	this->lift = 0.0f;
	this->forward = 0.0f;
	this->yaw = 0.0f;
	this->pitch = 0.0f;
	this->roll = 0.0f;
}


/**
* Called when a unit exits the vehicle
**/
void Helicopter::exit()
{
	this->running = false;
}


/**
* Called by the unit to update driving status
**/
void Helicopter::operate(Unit* u, int delta, bool keys[16], float horiz_angle, float vert_angle)
{
	float currY = this->getTransform().getOrigin().y();

	this->yaw = DEG_TO_RAD(horiz_angle);

	if (keys[Player::KEY_UP]) {
		this->forward = MIN(this->forward + 0.15f * delta, 3.0f);
		this->pitch = MIN(this->pitch + 0.002f * delta, PI / 8.0f);
	} else if (keys[Player::KEY_DOWN]) {
		this->forward = MAX(this->forward - 0.15f * delta, -3.0f);
		this->pitch = MAX(this->pitch - 0.002f * delta, 0.0f - PI / 8.0f);
	} else {
		this->forward = 0.0f;
		this->pitch = 0.0f;
	}
	
	if (keys[Player::KEY_LIFT] && currY < 50.0f) {
		this->lift = MIN(this->lift + 0.15f * delta, 2.0f);
	} else {
		this->lift = 0.0f;
	}

	if (currY > 10.0f) {
		if (keys[Player::KEY_RIGHT]) {
			this->roll = MIN(this->roll + 0.002f * delta, PI / 6.0f);
		} else if (keys[Player::KEY_LEFT]) {
			this->roll = MAX(this->roll - 0.002f * delta, PI / -6.0f);
		} else if (this->roll > 0.0f) {
			this->roll = MAX(this->roll - 0.002f * delta, 0.0f);
		} else if (this->roll < 0.0f) {
			this->roll = MIN(this->roll + 0.002f * delta, 0.0f);
		}
	}
}
