// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


Player::Player(UnitClass *uc, GameState *st) : Unit(uc, st)
{
	for (int i = 0; i < 4; i++) this->key[i] = 0;
	
	this->desired_angle_aim = 45;
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
	this->desired_angle_aim += (x - 400) / 2;		// 2 is the 'sensitivity' value
}


/**
* Uses the currently pressed keys to change the player movement
**/
void Player::update(int delta)
{
	UnitClassSettings *ucs = this->uc->getSettings(0);
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
		
	} else if (this->key[KEY_LEFT]) {
		this->desired_angle_move = 90;
		keypressed = true;
		
	} else if (this->key[KEY_RIGHT]) {
		this->desired_angle_move = 270;
		keypressed = true;
	}
	
	
	// A key was pressed
	if (keypressed) {
		this->speed += ppsDelta(ucs->lin_accel, delta);
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


