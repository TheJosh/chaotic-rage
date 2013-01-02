// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include "rage.h"
#include <math.h>

using namespace std;


Player::Player(UnitType *uc, GameState *st, float x, float y, float z) : Unit(uc, st, x, y, z)
{
	for (int i = 0; i < 16; i++) this->key[i] = 0;
	this->mouse_angle = 0.0f;
	this->vertical_angle = 0.0f;
}

Player::~Player()
{
}


/**
* Sets a key press
**/
void Player::keyPress(int idx)
{
	this->key[idx] = 1;
}


/**
* Sets a key release
**/
void Player::keyRelease(int idx)
{
	this->key[idx] = 0;
}


/**
* Packs a bitfield of keys
**/
Uint8 Player::packKeys()
{
	Uint8 k = 0;
	for (int i = 0; i < 8; i++) {
		k |= this->key[i] << i;
	}
	return k;
}


/**
* Set all keys.
* Bitfield should be a Uint8 of flags, with bit 0 => this->key[0], etc.
**/
void Player::setKeys(Uint8 bitfield)
{
	for (int i = 0; i < 8; i++) {
		this->key[i] = bitfield & (1 << i);
	}
}


/**
* Uses ->key and ->lkey to detect changes to the flags, and
* do the appropriate things
**/
void Player::handleKeyChange()
{
	if (this->key[KEY_FIRE] && !this->lkey[KEY_FIRE]) {
		if (this->lift_obj) {
			this->doDrop();
		} else {
			this->beginFiring();
		}
	} else if (!this->key[KEY_FIRE] && this->lkey[KEY_FIRE]) {
		this->endFiring();
	}

	if (this->key[KEY_USE] && !this->lkey[KEY_USE]) {
		this->doUse();
	}

	if (this->key[KEY_LIFT] && !this->lkey[KEY_LIFT]) {
		this->doLift();
	}
	
	if (this->key[KEY_MELEE] && !this->lkey[KEY_MELEE]) {
		this->meleeAttack();
	}

	if (this->key[KEY_SPECIAL] && !this->lkey[KEY_SPECIAL]) {
		this->specialAttack();
	}

	for (int i = 0; i < 16; i++) this->lkey[i] = this->key[i];
}


/**
* Sets the player angle to point towards the mouse
**/
void Player::angleFromMouse(int x, int y, int delta)
{
	float sensitivity = 5.0f;	// 1 = slow, 10 = nuts
	float change_dist;
	
	change_dist = x / (10.0f / sensitivity);
	this->mouse_angle = this->mouse_angle - change_dist;
	
	sensitivity *= 0.5f;
	change_dist = y / (10.0f / sensitivity);
	this->vertical_angle = this->vertical_angle - change_dist;
	if (this->vertical_angle > 10.0f) this->vertical_angle = 10.0f;
	if (this->vertical_angle < -10.0f) this->vertical_angle = -10.0f;
}


void Player::hasBeenHit(Entity * that)
{
	Unit::hasBeenHit(that);
	
	if (that->klass() == OBJECT) {
		this->curr_obj = (Object*)that;
		
		if (this->curr_obj->ot->over) this->doUse();
	}
}


/**
* Uses the currently pressed keys to change the player movement
**/
void Player::update(int delta)
{
	this->handleKeyChange();

	if (this->drive) {
		if (this->key[KEY_UP]) {
			this->drive->engineForce = MIN(this->drive->engineForce + 20.0f, 200.0f);		// TODO: Accel + Brake based on vt settings
			this->drive->brakeForce = 0.0f;

		} else if (this->key[KEY_DOWN]) {
			if (this->drive->getSpeedKmHr() > 0.0) {
				this->drive->brakeForce = MIN(this->drive->brakeForce + 10.0f, 50.0f);		// Brake
				this->drive->engineForce = 0.0f;
			} else {
				this->drive->engineForce = MAX(this->drive->engineForce - 5.0f, -30.0f);	// Reverse
				this->drive->brakeForce = 0.0f;
			}

		} else {
			this->drive->engineForce = MAX(this->drive->engineForce - 20.0f, 0.0f);			// Dampening
			this->drive->brakeForce = MAX(this->drive->brakeForce - 15.0f, 0.0f);
		}

		if (this->key[KEY_LEFT]) {
			this->drive->steering = MIN(this->drive->steering + 0.01f, 0.3f);
		} else if (this->key[KEY_RIGHT]) {
			this->drive->steering = MAX(this->drive->steering - 0.01f, -0.3f);
		} else if (this->drive->steering > 0.0f) {
			this->drive->steering = MAX(this->drive->steering - 0.01f, 0.0f);
		} else if  (this->drive->steering < 0.0f) {
			this->drive->steering = MIN(this->drive->steering + 0.01f, 0.0f);
		}

		return;
	}


	// NEW (kinematic controller)
	btVector3 walkDirection = btVector3(0.0, 0.0, 0.0);

	btScalar walkVelocity = btScalar(1.1) * 4.0; // 4 km/h -> 1.1 m/s
	btScalar walkSpeed = walkVelocity * delta/1000.0f;

	btTransform xform = ghost->getWorldTransform();

	// Mouse rotation
	btQuaternion rot = btQuaternion(btVector3(0.0f, 1.0f, 0.0f), DEG_TO_RAD(this->mouse_angle));
	ghost->getWorldTransform().setBasis(btMatrix3x3(rot));

	// Forward/backward
	btVector3 forwardDir = xform.getBasis() * btVector3(0.0f, 0.0f, uts->accel);
	if (this->key[KEY_UP]) {
		walkDirection += forwardDir;
	} else if (this->key[KEY_DOWN]) {
		walkDirection -= forwardDir;
	}

	// Strafe
	btVector3 strafeDir = xform.getBasis() * btVector3(uts->accel, 0.0f, 0.0f);
	if (this->key[KEY_LEFT]) {
		walkDirection += strafeDir;
	} else if (this->key[KEY_RIGHT]) {
		walkDirection -= strafeDir;
	}

	this->character->setWalkDirection(walkDirection * walkSpeed);


	// OLD (dynamic controller)
	/*
	btTransform xform, xform2;
	body->getMotionState()->getWorldTransform (xform);
	xform.setRotation(btQuaternion (btVector3(0.0f, 1.0f, 0.0f), DEG_TO_RAD(this->mouse_angle)));
	
	
	btVector3 linearVelocity = body->getLinearVelocity();
	btScalar speed = linearVelocity.length();
	
	DEBUG("unit", "%p Velocity: %f %f %f", this, linearVelocity.x(), linearVelocity.y(), linearVelocity.z());
	
	this->putOnGround(&xform);
	body->activate(true);

	if (!this->key[KEY_UP] && !this->key[KEY_DOWN] && !this->key[KEY_LEFT] && !this->key[KEY_RIGHT]) {
		linearVelocity *= btScalar(0.2);		// TODO: unit-type-settings?
		body->setLinearVelocity (linearVelocity);
		
	} else if (speed < uts->max_speed) {	
		xform2 = xform;
		btVector3 walkDirection = btVector3(0.0f, 0.0f, 0.0f);

		// Forward/Backwards
		btVector3 forwardDir = xform2.getBasis() * btVector3(0.0f, 0.0f, uts->accel);
		
		if (this->key[KEY_UP]) {
			walkDirection += forwardDir;
		} else if (this->key[KEY_DOWN]) {
			walkDirection -= forwardDir;
		}
		
		// Strafe
		btVector3 strafeDir = xform2.getBasis() * btVector3(uts->accel, 0.0f, 0.0f);
		
		if (this->key[KEY_LEFT]) {
			walkDirection += strafeDir;
		} else if (this->key[KEY_RIGHT]) {
			walkDirection -= strafeDir;
		}
		
		// Hill climbing
		btVector3 upDir = xform2.getBasis() * btVector3(0.0f, 0.7f, 0.0f); 
		walkDirection += upDir;

		// Apply velocity
		linearVelocity += walkDirection;
		body->setLinearVelocity (linearVelocity);
	}
	

	body->getMotionState()->setWorldTransform (xform);
	body->setCenterOfMassTransform (xform);
	
	*/


	Unit::update(delta);
}


int Player::takeDamage(int damage)
{
	int result = Unit::takeDamage(damage);
	
	if (result == 1) {
		this->st->logic->raise_playerdied(this->slot);
		
		for (unsigned int i = 0; i < this->st->num_local; i++) {
			if (this == this->st->local_players[i]->p) {
				this->st->local_players[i]->p = NULL;
			}
		}
	}
	
	return result;
}

