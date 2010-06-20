#include <iostream>
#include <algorithm>
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
	this->particles.push_back(particle);
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
			
			vector<Particle*>::iterator it;
			it = find (this->particles.begin(), this->particles.end(), e);
			if (it != this->particles.end()) {
				this->particles.erase(it);
			}
			
		} else {
			e->update(delta);
		}
	}
	
	this->game_time += delta;
	this->anim_frame = floor(this->game_time / ANIMATION_FPS);
}



vector<Particle*> * GameState::particlesInside(int x, int y, int w, int h)
{
	unsigned int i;
	vector<Particle*> * ret = new vector<Particle*>();
	
	for (i = 0; i < this->particles.size(); i++) {
		Particle *p = this->particles.at(i);
		
		if (p->x >= x && p->x <= x + w && p->y >= y && p->y <= y + h) {
			ret->push_back(p);
		}
	}
	
	return ret;
}

