// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <string.h>
#include <iostream>
#include <map>
#include <algorithm>
#include <SDL.h>
#include <math.h>
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
	this->num_local = 0;
	this->curr_slot = 1;
	this->reset_mouse = false;

	for (unsigned int i = 0; i < MAX_LOCAL; i++) {
		this->local_players[i] = NULL;
	}

	this->render = NULL;
	this->hud = NULL;
	this->audio = NULL;
	this->logic = NULL;
	this->collides = NULL;
	this->client = NULL;
	this->server = NULL;
	
	g_st = this;
}

GameState::~GameState()
{
}


/**
* Add a unit
**/
void GameState::addUnit(Unit* unit)
{
	this->entities_add.push_back(unit);
	this->units.push_back(unit);
	
	if (this->server) this->server->addmsgUnitAdd(unit);
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
* Add an object
**/
void GameState::addObject(Object* object)
{
	this->entities_add.push_back(object);
}


/**
* Finds the unit which matches a given slot
**/
Unit* GameState::findUnitSlot(int slot)
{
	if (slot <= 0) return NULL;
	
	for (list<Entity*>::iterator it = this->entities.begin(); it != this->entities.end(); it++) {
		if ((*it)->klass() != UNIT) continue;
		if (((Unit*)*it)->slot == slot) return ((Unit*)*it);
	}
	
	return NULL;
}


/**
* Used for filtering
**/
static bool EntityEraser(Entity* e)
{
	if (e->del == false) return false;
	delete e;
	return true;
}

/**
* Used for filtering
**/
static bool CollideBoxEraser(CollideBox* box)
{
	if (box->del == false) return false;
	delete box;
	return true;
}


/**
* Gets the entropy for a given player
**/
unsigned int GameState::getEntropy(unsigned int slot)
{
	return this->entropy;
}

/**
* Increases the entropy for a player
**/
void GameState::increaseEntropy(unsigned int slot)
{
	this->entropy += 10;
}



/**
* Clears all game state, ready for a new game
**/
void GameState::clear()
{
	// TODO: Are these leaky?
	this->entities.clear();
	this->entities_add.clear();
	this->collideboxes.clear();
	
	// TODO: Are these needed?
	this->units.clear();
	this->walls.clear();
}


/**
* Game has started
**/
void GameState::start()
{
	this->collides = new MapGrid(curr_map->width, curr_map->height);
	this->collideboxes.clear();
	
	// It should technically be 0, but 1 avoids division-by-zero
	this->game_time = 1;
	this->anim_frame = 1;
	this->reset_mouse = false;
}


/**
* Updates the state of everything
**/
void GameState::update(int delta)
{
	DEBUG("loop", "Updating gamestate using delta: %i\n", delta);
	
	
	// Add new entities
	this->entities.insert(
		this->entities.end(),
		this->entities_add.begin(),
		this->entities_add.end()
	);
	this->entities_add.clear();
	
	// Update entities
	for (list<Entity*>::iterator it = this->entities.begin(); it != this->entities.end(); it++) {
		Entity *e = (*it);
		if (! e->del) {
			e->update(delta);
		}
	}
	
	// Remove stuff
	this->entities.remove_if(EntityEraser);
	this->collideboxes.remove_if(CollideBoxEraser);
	
	// Update physics
	this->physics->stepTime(delta);
	
	// What hit what?
	this->physics->doCollisions();
	
	// Decrease entropy
	if (this->entropy > 0) {
		this->entropy--;
	}
	
	// Update time
	this->game_time += delta;
	this->anim_frame = (int) floor(this->game_time * ANIMATION_FPS / 1000.0);
}


/**
* Finds units visible to this unit.
*
* Returns a UnitQueryResult. Don't forget to delete() it when you are done!
**/
list<UnitQueryResult> * GameState::findVisibleUnits(Unit* origin)
{
	list<UnitQueryResult> * ret = new list<UnitQueryResult>();
	
	float visual_distance = 60;		// TODO: Should this be a unit property?
	
	btTransform trans;
	UnitQueryResult uqr;
	btVector3 vecO, vecS;
	float dist;
	Unit * u;
	
	origin->getRigidBody()->getMotionState()->getWorldTransform(trans);
	vecO = trans.getOrigin();
	
	for (list<Entity*>::iterator it = this->entities.begin(); it != this->entities.end(); it++) {
		if ((*it)->klass() != UNIT) continue;
		u = (Unit*)(*it);
		
		u->getRigidBody()->getMotionState()->getWorldTransform(trans);
		vecS = trans.getOrigin();
		vecS -= vecO;
		
		dist = vecS.length();
		if (dist > visual_distance) continue;
		
		uqr.u = u;
		uqr.dist = dist;
		
		ret->push_back(uqr);
	}
	
	return ret;
}


