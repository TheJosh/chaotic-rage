#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


GameState::GameState()
{
	this->anim_frame = 0;
	this->game_time = 0;
}

GameState::~GameState()
{
}


void GameState::addUnit(Unit* unit)
{
	this->units.push_back(unit);
}

void GameState::addParticle(Particle* particle)
{
	this->particles.push_back(particle);
}


/**
* Updates the state of everything
**/
void GameState::update(int delta)
{
	unsigned int i;
	
	DEBUG("Updating gamestate using delta: %i\n", delta);
	
	// Units
	for (i = 0; i < this->units.size(); i++) {
		Unit *e = this->units.at(i);
		e->update(delta);
	}
	
	// Particles
	for (i = 0; i < this->particles.size(); i++) {
		Particle *e = this->particles.at(i);
		e->update(delta);
	}
	
	this->game_time += delta;
	this->anim_frame = floor(this->game_time / ANIMATION_FPS);
}


