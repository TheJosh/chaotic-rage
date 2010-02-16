#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


Player::Player()
{
	this->sprites = (SDL_Surface**) malloc(sizeof(SDL_Surface*));
	
	this->sprites[0] = loadSprite("sprites/player/player_180deg_static.bmp");
	
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
	if (this->key[KEY_FWD]) {
		this->speed += 30;
	} else if (this->key[KEY_REV]) {
		this->speed -= 30;
	} else {
		this->speed = 0;
	}
	
	if (this->speed > 350) this->speed = 350;
	if (this->speed < -150) this->speed = -150;
	
	Unit::update(delta);
}


