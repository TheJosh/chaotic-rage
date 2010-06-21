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
	this->entities_add.push_back(unit);
}

void GameState::addParticle(Particle* particle)
{
	this->entities_add.push_back(particle);
	this->particles.push_back(particle);
}

void GameState::addParticleGenerator(ParticleGenerator* generator)
{
	this->entities_add.push_back(generator);
}



bool EntityEraser(Entity *e)
{
	return e->del;
}

/**
* Updates the state of everything
**/
void GameState::update(int delta)
{
	DEBUG("Updating gamestate using delta: %i\n", delta);
	
	vector<Entity*>::iterator it;
	vector<Entity*>::iterator newend;
	
	// Add new entities
	for (it = this->entities_add.begin(); it < this->entities_add.end(); it++) {
		Entity *e = (*it);
		this->entities.push_back(e);
	}
	this->entities_add.clear();
	
	// Update entities
	for (it = this->entities.begin(); it < this->entities.end(); it++) {
		Entity *e = (*it);
		e->update(delta);
	}
	
	// Remove entities
	// I'm fairly sure this leaks because it doesn't actually delete the entity
	newend = remove_if(this->entities.begin(), this->entities.end(), EntityEraser);
	this->entities.erase(newend, this->entities.end());
	
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

