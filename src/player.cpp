#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


Player::Player()
{
	this->sprites = (SDL_Surface*) malloc(sizeof(SDL_Surface));
	
	this->sprites[0] = *SDL_LoadBMP("sprites/player/player_180deg_static.bmp");
	
	return;
}

Player::~Player()
{
	return;
}


SDL_Surface* Player::getSprite()
{
	return &this->sprites[0];
}
