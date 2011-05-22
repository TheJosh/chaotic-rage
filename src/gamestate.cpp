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
	
	this->hud = new HUD();
	this->hud->st = this;
	
	this->render = NULL;
	
	this->reset_mouse = true;
	
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
static bool EntityEraser(Entity* e)
{
	if (! e->del) return false;
	cout << "EntityEraser()" << "\n";
	delete e;
	return true;
}

/**
* Used for filtering
**/
static bool CollideBoxEraser(CollideBox* box)
{
	if (! box->del) return false;
	cout << "CollideBoxEraser()" << "\n";
	delete box;
	return true;
}


/**
* Game has started
**/
void GameState::start()
{
	this->collides = new MapGrid(curr_map->width, curr_map->height);
	this->collideboxes.clear();
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
	
	// Remove collideboxes
	this->collideboxes.remove_if(CollideBoxEraser);
	
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
	int dist;
	int search;
	
	for (list<CollideBox*>::iterator ito = this->collideboxes.begin(); ito != this->collideboxes.end(); ito++) {
		CollideBox *co = (*ito);
		
		list<CollideBox*>* tests = this->collides->getCollidesMC(co->x, co->y, co->radius);
		if (tests->size() < 2) {
			delete tests;
			continue;
		}
		
		for (list<CollideBox*>::iterator iti = tests->begin(); iti != tests->end(); iti++) {
			CollideBox *ci = (*iti);
			
			if (ci == co) continue;
			
			dist = (int) floor(sqrt((float)((ci->x - co->x) * (ci->x - co->x)) + ((ci->y - co->y) * (ci->y - co->y))));
			search = ci->radius + co->radius;
			
			if (dist <= (ci->radius + co->radius)) {
				co->e->hasBeenHit(co, ci);
			}
		}
		
		delete tests;
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
* Get a mod
**/
Mod * GameState::getMod(int id)
{
	return mods.at(id);
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
	cout << "delCollideBox()" << "\n";
	if (box == NULL) return;
	
	MapGridCell* cell = this->collides->getCellMC(box->x, box->y);
	cell->collideboxes.remove(box);
	
	box->del = true;
}



