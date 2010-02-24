#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"

using namespace std;


Particle::Particle(ParticleType *pt, GameState *st) : Entity(st)
{
	this->pt = pt;
}

Particle::~Particle()
{
}


void Particle::update(int delta)
{

}

SDL_Surface* Particle::getSprite()
{
	return this->pt->sprites.at(0);
}


