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
	if (this->drive_obj != NULL && this->speed == 0) return;	// Cars can only turn while moving

	// TODO: Think about delta
	
	float sensitivity = 5.0; // 1 = slow, 10 = nuts
	
	float change_dist = x;
	change_dist /= (10.0 / sensitivity);
	
	//float changeAngle = asin(changeDist*(3.142/180.0))*(180/3.142);
	
	this->angle_aim = this->angle_aim - change_dist;
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
	UnitTypeSettings *ucs = this->uc->getSettings(0);
	bool keypressed = false;
	
	
	// Up/Down
	if (this->key[KEY_UP]) {
		if (this->key[KEY_LEFT]) {
			this->desired_angle_move = 45;
		} else if (this->key[KEY_RIGHT]) {
			this->desired_angle_move = 315;
		} else {
			this->desired_angle_move = 0;
		}
		keypressed = true;
		
	} else if (this->key[KEY_DOWN]) {
		if (this->key[KEY_LEFT]) {
			this->desired_angle_move = 135;
		} else if (this->key[KEY_RIGHT]) {
			this->desired_angle_move = 225;
		} else {
			this->desired_angle_move = 180;
		}
		keypressed = true;
		
	} else if (this->key[KEY_LEFT] && this->drive_obj == NULL) {
		this->desired_angle_move = 90;
		keypressed = true;
		
	} else if (this->key[KEY_RIGHT] && this->drive_obj == NULL) {
		this->desired_angle_move = 270;
		keypressed = true;
	}
	
	
	
	{
		cout << "-----------------------------\n";
		
		cout << "angle: " << this->angle << "\n";
		
		btTransform xform;
		body->getMotionState()->getWorldTransform (xform);
		
		xform.setRotation (btQuaternion (btVector3(0.0, 0.0, 1.0), DEG_TO_RAD(this->angle)));
		
		
		btVector3 linearVelocity = body->getLinearVelocity();
		btScalar speed = linearVelocity.length();
		
		
		cout << "initial: " << linearVelocity.x() << " " << linearVelocity.y() << " " << linearVelocity.z() << "\n";
		cout << "speed: " << speed << "\n";
		
		if (!this->key[KEY_UP] && !this->key[KEY_DOWN]) {
			linearVelocity *= btScalar(0.2);
			
			cout << "scaled: " << linearVelocity.x() << " " << linearVelocity.y() << " " << linearVelocity.z() << "\n";
			
			body->setLinearVelocity (linearVelocity);
			
		} else if (speed < 10.0) {
			body->activate(true);
			
			btVector3 forwardDir = xform.getBasis()[1];
			forwardDir.normalize ();
			forwardDir *= btScalar(2.5);
			
			cout << "forward: " << forwardDir.x() << " " << forwardDir.y() << " " << forwardDir.z() << "\n";
			
			btVector3 walkDirection = btVector3(0.0, 0.0, 0.0);
		
			if (this->key[KEY_UP]) {
				walkDirection -= forwardDir;
			} else if (this->key[KEY_DOWN]) {
				walkDirection += forwardDir;
			}
			
			cout << "walk: " << walkDirection.x() << " " << walkDirection.y() << " " << walkDirection.z() << "\n";
			
			btVector3 velocity = linearVelocity + walkDirection;
			
			cout << "pluswalk: " << velocity.x() << " " << velocity.y() << " " << velocity.z() << "\n";
			
			body->setLinearVelocity (velocity);
		}
	}
	
	
	
	this->desired_angle_move += this->angle_aim;
	
	
	// A movement key was pressed
	if (keypressed) {
		this->speed += ppsDeltaf(ucs->accel, delta);
		this->setState(UNIT_STATE_RUNNING);
		
	} else if (speed > 0) {		// nothing pressed, slow down (forwards)
		this->speed -= (speed > 100 ? 100 : speed);
		this->setState(UNIT_STATE_STATIC);
		
	} else if (speed < 0) {		// nothing pressed, slow down (reverse)
		this->speed += (0 - speed > 100 ? 100 : 0 - speed);
		this->setState(UNIT_STATE_STATIC);
	}
	
	// Bound to limits
	if (this->speed > ucs->max_speed) this->speed = ucs->max_speed;
	if (this->speed < 0 - ucs->max_speed) this->speed = 0 - ucs->max_speed;
	
	
	Unit::update(delta, ucs);
	
	delete ucs;
}


int Player::takeDamage(int damage)
{
	int result = Unit::takeDamage(damage);
	
	if (result == 1) {
		this->st->logic->raise_playerdied();
		
		for (unsigned int i = 0; i < this->st->num_local; i++) {
			if (this == this->st->local_players[i]) {
				this->st->local_players[i] = NULL;
			}
		}
	}
	
	return result;
}

