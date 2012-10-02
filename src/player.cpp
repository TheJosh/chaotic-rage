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

	if (idx == KEY_FIRE) {
		if (this->lift_obj) {
			this->doDrop();
		} else {
			this->beginFiring();
		}

	} else if (idx == KEY_USE) {
		this->doUse();
	} else if (idx == KEY_LIFT) {
		this->doLift();
	} else if (idx == KEY_MELEE) {
		this->meleeAttack();
	} else if (idx == KEY_SPECIAL) {
		this->specialAttack();
	}
}


/**
* Sets a key release
**/
void Player::keyRelease(int idx)
{
	this->key[idx] = 0;

	if (idx == KEY_FIRE) {
		this->endFiring();
	}
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
* Sets the player angle to point towards the mouse
**/
void Player::angleFromMouse(int x, int y, int delta)
{
	float sensitivity = 5.0f;	// 1 = slow, 10 = nuts
	float change_dist;
	
	change_dist = x / (10.0f / sensitivity);
	this->mouse_angle = this->mouse_angle + change_dist;
	
	change_dist = y / (10.0f / sensitivity / 2.0f);
	this->vertical_angle = this->vertical_angle + change_dist;
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
	if (this->drive) {
		if (this->key[KEY_UP]) {
			this->drive->engineForce = MIN(this->drive->engineForce + 20.0f, 200.0f);		// TODO: Accel + Brake based on vt settings
			this->drive->brakeForce = 0.0f;

		} else if (this->key[KEY_DOWN]) {
			if (this->drive->getSpeedKmHr() > 0.0) {
				this->drive->brakeForce = MIN(this->drive->brakeForce + 15.0f, 100.0f);
				this->drive->engineForce = 0.0f;
			} else {
				this->drive->engineForce = MAX(this->drive->engineForce - 5.0f, -30.0f);
				this->drive->brakeForce = 0.0f;
			}

		} else {
			this->drive->engineForce = MAX(this->drive->engineForce - 20.0f, 0.0f);
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


	btTransform xform, xform2;
	body->getMotionState()->getWorldTransform (xform);
	xform2 = xform;
	
	
	btVector3 linearVelocity = body->getLinearVelocity();
	btScalar speed = linearVelocity.length();
	
	DEBUG("unit", "%p Velocity: %f %f %f", this, linearVelocity.x(), linearVelocity.y(), linearVelocity.z());
	

	if (!this->onground()) {
		linearVelocity += btVector3(0.0f, 0.0f, -0.7f);
		body->setLinearVelocity (linearVelocity);

	} else if (!this->key[KEY_UP] && !this->key[KEY_DOWN] && !this->key[KEY_LEFT] && !this->key[KEY_RIGHT]) {
		linearVelocity *= btScalar(0.2);		// TODO: unit-type-settings?
		body->setLinearVelocity (linearVelocity);
		
	} else if (speed < uts->max_speed) {
		body->activate(true);
		
		xform2.setRotation (btQuaternion (btVector3(0.0, 0.0, 1.0), DEG_TO_RAD(this->mouse_angle)));
		btVector3 forwardDir = xform2.getBasis()[1];
		forwardDir.normalize();
		forwardDir *= btScalar(uts->accel);
		
		btVector3 walkDirection = btVector3(0.0, 0.0, 0.0);
		
		if (this->key[KEY_UP]) {
			walkDirection += forwardDir;
		} else if (this->key[KEY_DOWN]) {
			walkDirection -= forwardDir;
		}
		
		xform2.setRotation (btQuaternion (btVector3(0.0, 0.0, 1.0), DEG_TO_RAD(this->mouse_angle + 90)));
		btVector3 strafeDir = xform2.getBasis()[1];
		strafeDir.normalize();
		strafeDir *= btScalar(0.7);		// TODO: Managed in the unit settings	(strafe accel)
		
		if (this->key[KEY_LEFT]) {
			walkDirection -= strafeDir;
		} else if (this->key[KEY_RIGHT]) {
			walkDirection += strafeDir;
		}
		
		linearVelocity += walkDirection;
		linearVelocity += btVector3(0.0f, 0.0f, 0.7f);		// hill climbing

		body->setLinearVelocity (linearVelocity);
	}
	

	body->getMotionState()->setWorldTransform (xform);
	body->setCenterOfMassTransform (xform);
	
	
	Unit::update(delta);
}


int Player::takeDamage(int damage)
{
	int result = Unit::takeDamage(damage);
	
	if (result == 1) {
		this->st->logic->raise_playerdied();
		
		for (unsigned int i = 0; i < this->st->num_local; i++) {
			if (this == this->st->local_players[i]->p) {
				this->st->local_players[i]->p = NULL;
			}
		}
	}
	
	return result;
}

