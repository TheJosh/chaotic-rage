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
*
* Should this be removed?
**/
GameState * getGameState()
{
	return g_st;
}


/**
* New game state
**/
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
	delete(this->hud);
}


/**
* Add a unit
**/
void GameState::addUnit(Unit* unit)
{
	this->entities_add.push_back(unit);
	this->units.push_back(unit);
}

/**
* Add a particle
**/
void GameState::addParticle(Particle* particle)
{
	this->entities_add.push_back(particle);
}

/**
* Add a particle generator
**/
void GameState::addParticleGenerator(ParticleGenerator* generator)
{
	this->entities_add.push_back(generator);
}

/**
* Add a wall
**/
void GameState::addWall(Wall* wall)
{
	this->entities_add.push_back(wall);
	this->walls.push_back(wall);
}


/**
* Used for filtering
**/
static bool EntityEraser(Entity *e)
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
	
	this->game_time += delta;
	this->anim_frame = (int) floor(this->game_time * ANIMATION_FPS / 1000.0);
}


/**
* Add a mod to the list
**/
void GameState::addMod(Mod * mod)
{
	mods.push_back(mod);
}

/**
* Get a mod
**/
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
	vector<Wall*>::iterator it;
	for (it = this->walls.begin(); it < this->walls.end(); it++) {
		Wall *e = (*it);
		if (e->health == 0) continue;
		
		int dist = (int) ceil(sqrt(((x - e->x) * (x - e->x)) + ((y - e->y) * (y - e->y))));
		
		if (dist < (check_radius + e->wt->check_radius)) {
			return e;
		}
	}
	
	return NULL;
}

/**
* Basically the same as above
*
* @todo Non-static 'check_radius' figure
* @todo Merge into the above, and make awesome
**/
Unit * GameState::checkHitUnit(float x, float y, int check_radius)
{
	vector<Unit*>::iterator it;
	for (it = this->units.begin(); it < this->units.end(); it++) {
		Unit *e = (*it);
		
		int dist = (int) ceil(sqrt(((x - e->x) * (x - e->x)) + ((y - e->y) * (y - e->y))));
		
		if (dist < (check_radius + 30)) {
			return e;
		}
	}
	
	return NULL;
}

