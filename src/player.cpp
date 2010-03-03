#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


Player::Player(UnitClass *uc, GameState *st) : Unit(uc, st)
{
	this->key[KEY_FWD] = 0;
	this->key[KEY_REV] = 0;
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
* Sets the player angle to point towards the mouse
**/
void Player::angleFromMouse(int x, int y)
{
	this->desired_angle = getAngleBetweenPoints(this->x, this->y, x, y);
}


/**
* Uses the currently pressed keys to change the player movement
**/
void Player::update(int delta)
{
	UnitClassSettings *ucs = this->uc->getSettings(0);
	
	if (this->key[KEY_FWD]) {	// fwd key pressed
		this->speed += ppsDelta(ucs->lin_accel, delta);
		this->setState(UNIT_STATE_RUNNING);
		
	} else if (this->key[KEY_REV]) {	// rev key pressed
		this->speed -= ppsDelta(ucs->lin_accel, delta);
		this->setState(UNIT_STATE_RUNNING);
		
	} else if (speed > 0) {		// nothing pressed, slow down (forwards)
		this->speed -= (speed > 100 ? 100 : speed);
		this->setState(UNIT_STATE_STATIC);
		
	} else if (speed < 0) {		// nothing pressed, slow down (reverse)
		this->speed += (0 - speed > 100 ? 100 : 0 - speed);
		this->setState(UNIT_STATE_STATIC);
	}
	
	// Bound to limits
	if (this->speed > ucs->lin_speed) this->speed = ucs->lin_speed;
	if (this->speed < 0 - ucs->lin_speed) this->speed = 0 - ucs->lin_speed;
	
	Unit::update(delta, ucs);
	
	delete ucs;
}


