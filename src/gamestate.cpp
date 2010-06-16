#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


GameState::GameState()
{
	this->anim_frame = 0;
	this->game_time = 0;
	
	this->hud = new HUD();
	this->hud->st = this;
}

GameState::~GameState()
{
}


void GameState::addUnit(Unit* unit)
{
	this->entities.push_back(unit);
}

void GameState::addParticle(Particle* particle)
{
	this->entities.push_back(particle);
}

void GameState::addParticleGenerator(ParticleGenerator* generator)
{
	this->entities.push_back(generator);
}


/**
* Updates the state of everything
**/
void GameState::update(int delta)
{
	unsigned int i;
	
	DEBUG("Updating gamestate using delta: %i\n", delta);
	
	// Update all entities
	for (i = 0; i < this->entities.size(); i++) {
		Entity *e = this->entities.at(i);
		
		if (e->del) {
			delete (e);
			this->entities.erase(this->entities.begin() + i);
		} else {
			e->update(delta);
		}
	}
	
	this->game_time += delta;
	this->anim_frame = floor(this->game_time / ANIMATION_FPS);
}


