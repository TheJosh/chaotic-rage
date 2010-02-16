#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


Player::Player(UnitClass *uc) : Unit(uc)
{
	this->sprites = (SDL_Surface**) malloc(sizeof(SDL_Surface*));
	
	this->sprites[0] = loadSprite("unitclass/marine/player_180deg_static.bmp");
	
	this->key[KEY_FWD] = 0;
	this->key[KEY_REV] = 0;
}

Player::~Player()
{
	SDL_FreeSurface(this->sprites[0]);
}


void Player::keyPress(int idx)
{
	this->key[idx] = 1;
}

void Player::keyRelease(int idx)
{
	this->key[idx] = 0;
}

void Player::angleFromMouse(int x, int y)
{
	this->desired_angle = getAngleBetweenPoints(this->x, this->y, x, y);
}


SDL_Surface* Player::getSprite()
{
	return this->sprites[0];
}

void Player::update(int delta)
{
	UnitClassSettings *ucs = this->uc->getSettings(0);
	
	if (this->key[KEY_FWD]) {	// fwd key pressed
		this->speed += ppsDelta(ucs->lin_accel, delta);
		
	} else if (this->key[KEY_REV]) {	// rev key pressed
		this->speed -= ppsDelta(ucs->lin_accel, delta);
		
	} else if (speed > 0) {		// nothing pressed, slow down (forwards)
		this->speed -= (speed > 100 ? 100 : speed);
		
	} else if (speed < 0) {		// nothing pressed, slow down (reverse)
		this->speed += (0 - speed > 100 ? 100 : 0 - speed);
	}
	
	
	// Bound to limits
	if (this->speed > ucs->lin_speed) this->speed = ucs->lin_speed;
	if (this->speed < 0 - ucs->lin_speed) this->speed = 0 - ucs->lin_speed;
	
	Unit::update(delta, ucs);
	
	delete ucs;
}


