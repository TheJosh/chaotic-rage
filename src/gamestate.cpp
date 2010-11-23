// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <algorithm>
#include <SDL.h>
#include "rage.h"

using namespace std;


static GameState *g_st;


static bool EntityEraser(Entity *e);


/**
* Don't use unless if you are in global code
**/
GameState * getGameState()
{
	return g_st;
}


GameState::GameState()
{
	this->anim_frame = 0;
	this->game_time = 0;
	
	this->hud = new HUD();
	this->hud->st = this;
	
	this->render = NULL;
	
	g_st = this;
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

void GameState::addWall(Wall* wall)
{
	this->entities_add.push_back(wall);
}


static bool EntityEraser(Entity *e)
{
	return e->del;
}

static bool ParticleEraser(Particle *e)
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
		if (! e->del) {
			e->update(delta);
		}
	}
	
	// Remove entities
	// I'm fairly sure this leaks because it doesn't actually delete the entity
	newend = remove_if(this->entities.begin(), this->entities.end(), EntityEraser);
	this->entities.erase(newend, this->entities.end());
	
	// Remove particles from the particles list
	{
		vector<Particle*>::iterator it = remove_if(this->particles.begin(), this->particles.end(), ParticleEraser);
		this->particles.erase(it, this->particles.end());
	}
	
	this->game_time += delta;
	this->anim_frame = floor(this->game_time * ANIMATION_FPS / 1000.0);
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


void GameState::addMod(Mod * mod)
{
	mods.push_back(mod);
}

Mod * GameState::getMod(int id)
{
	return mods.at(id);
}


/**
* Returns an AreaType if we hit a wall, or NULL if we haven't
*
* Makes use of the AreaType check radius, as well as the checking objects'
* check radius.
*
* @tag good-for-optimise
* @todo rehash for epicenter positioning
**/
Wall * GameState::checkHitWall(float x, float y, int check_radius)
{
	vector<Entity*>::iterator it;
	for (it = this->entities.begin(); it < this->entities.end(); it++) {
		Entity *e = (*it);
		if (e->klass() != WALL) continue;
		if (((Wall*)e)->health == 0) continue;
		
		int dist = ceil(sqrt(((x - e->x) * (x - e->x)) + ((y - e->y) * (y - e->y))));
		
		if (dist < (check_radius + ((Wall*)e)->wt->check_radius)) {
			return (Wall*) e;
		}
	}
	
	return NULL;
}

