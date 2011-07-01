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
	this->curr_player = NULL;
	this->reset_mouse = false;
	
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
	this->curr_player = NULL;
	this->reset_mouse = false;
}


/**
* Updates the state of everything
**/
void GameState::update(int delta)
{
	DEBUG("Updating gamestate using delta: %i\n", delta);
	
	
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
	
	// What hit what?
	this->doCollisions();
	
	// Update time
	this->game_time += delta;
	this->anim_frame = (int) floor(this->game_time * ANIMATION_FPS / 1000.0);
}


/**
* Look for collissions of entities
**/
void GameState::doCollisions()
{
	int release;
	
	for (list<CollideBox*>::iterator ito = this->collideboxes.begin(); ito != this->collideboxes.end(); ito++) {
		CollideBox *co = (*ito);
		
		list<CollideBox*>* tests = this->collides->getCollidesMC(co->x, co->y, co->radius, &release);
		
		for (list<CollideBox*>::iterator iti = tests->begin(); iti != tests->end(); iti++) {
			CollideBox *ci = (*iti);
			
			if (ci == co) continue;
			
			
			// Square based check. This could be faster.
			/*
			int xi1 = ci->x - ci->radius;
			int xi2 = ci->x + ci->radius;
			int yi1 = ci->y - ci->radius;
			int yi2 = ci->y + ci->radius;
			
			int xo1 = co->x - co->radius;
			int xo2 = co->x + co->radius;
			int yo1 = co->y - co->radius;
			int yo2 = co->y + co->radius;
			
			
			if (
				((xo1 > xi1 and xo1 < xi2) or (xo2 > xi1 and xo2 < xi2))
				and
				((yo1 > yi1 and yo1 < yi2) or (yo2 > yi1 and yo2 < yi2))
			) {
				co->e->hasBeenHit(co, ci);
			}
			*/
			
			
			// Circle based check using sqrt. This is probably too slow.
			int dist = (int) floor(sqrt((float)((ci->x - co->x) * (ci->x - co->x)) + ((ci->y - co->y) * (ci->y - co->y))));
			if (dist <= (ci->radius + co->radius)) {
				co->e->hasBeenHit(co, ci);
			}
		}
		
		if (release == 1) delete tests;
	}
}


/**
* Add a mod to the list
**/
void GameState::addMod(Mod * mod)
{
	mods.push_back(mod);
}

/**
* Get a mod via name
**/
Mod * GameState::getMod(string name)
{
	if (name.empty()) return NULL;
	
	int i;
	for (i = mods.size() - 1; i >= 0; --i) {
		if (mods.at(i)->name.compare(name) == 0) return mods.at(i);
	}
	
	reportFatalError("Data module is not loaded: " + name);
	return NULL;
}


/**
* Get the 'default' mod
**/
Mod * GameState::getDefaultMod()
{
	return mods.at(0);
}


/**
* Only run this if you are in an 'update' method for an entity.
*
* @param x, y		The centerpoint of the collidebox
* @param radius		The size of the collidebox
* @param e			The entity assoc. with this collidebox
* @param cares		If true, collissions send events to the entity
**/
CollideBox* GameState::addCollideBox(int x, int y, int radius, Entity *e, bool cares)
{
	if (x < 0 or x >= curr_map->width) return NULL;
	if (y < 0 or y >= curr_map->height) return NULL;
	
	CollideBox * box = new CollideBox(x, y, radius, e);
	
	MapGridCell* cell = this->collides->getCellMC(x, y);
	cell->collideboxes.push_back(box);
	
	if (cares) {
		this->collideboxes.push_back(box);
	}
	
	return box;
}


/**
* Moves the collidebox to a new cell
**/
void GameState::moveCollideBox(CollideBox* box, int x, int y)
{
	if (box == NULL) return;
	if (box->x == x and box->y == y) return;
	if (x < 0 or x >= curr_map->width) return;		// should this delete instead?
	if (y < 0 or y >= curr_map->height) return;
	
	MapGridCell* cell;
	
	cell = this->collides->getCellMC(box->x, box->y);
	cell->collideboxes.remove(box);
	
	box->x = x;
	box->y = y;
	
	cell = this->collides->getCellMC(box->x, box->y);
	cell->collideboxes.push_back(box);
}


/**
* Changes the radius of a collide box
**/
void GameState::sizeCollideBox(CollideBox* box, int radius)
{
	if (box == NULL) return;
	
	box->radius = radius;
}


/**
* Removes a collide box
**/
void GameState::delCollideBox(CollideBox* box)
{
	if (box == NULL) return;
	
	MapGridCell* cell = this->collides->getCellMC(box->x, box->y);
	cell->collideboxes.remove(box);
	
	box->del = true;
}



