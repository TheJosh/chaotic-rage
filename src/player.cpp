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
* Determines the player sprite
*
* TODO: move into unit.cpp
**/
SDL_Surface* Player::getSprite()
{
	int idx = round(this->angle / 45);
	
	int frame = this->st->anim_frame - this->animation_start;
	frame = frame % this->current_state->num_frames;
	
	idx *= this->uc->getMaxFrames();
	idx += frame;
	
	idx += this->current_state->sprite_offset;
	
	return this->sprites->at(idx);
}


/**
* Uses the currently pressed keys to change the player movement
*
* TODO: move stuff into unit.cpp if appropriate
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
		
	} else if (speed < 0) {		// nothing pressed, slow down (reverse)
		this->speed += (0 - speed > 100 ? 100 : 0 - speed);
	}
	
	// Standing still...
	if (this->speed == 0) {
		this->setState(UNIT_STATE_STATIC);
	}
	
	// Bound to limits
	if (this->speed > ucs->lin_speed) this->speed = ucs->lin_speed;
	if (this->speed < 0 - ucs->lin_speed) this->speed = 0 - ucs->lin_speed;
	
	// Firing
	if (this->key[KEY_FIRE]) {
		this->beginFiring();
	} else {
		this->endFiring();
	}
	
	Unit::update(delta, ucs);
	
	delete ucs;
}


