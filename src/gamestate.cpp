// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <string.h>
#include <iostream>
#include <map>
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
	this->curr_player = NULL;
	
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
	
	this->collides = new MapGrid(curr_map->width, curr_map->height);
	this->collideboxes.clear();
	
	
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
	
	this->doCollisions();
	
	// Remove entities
	// I'm fairly sure this leaks because it doesn't actually delete the entity
	newend = remove_if(this->entities.begin(), this->entities.end(), EntityEraser);
	this->entities.erase(newend, this->entities.end());
	
	// Update time
	this->game_time += delta;
	this->anim_frame = (int) floor(this->game_time * ANIMATION_FPS / 1000.0);
	
	
	delete(this->collides);
}


/**
* Look for collissions of entities
**/
void GameState::doCollisions()
{
	float dist;
	
	cout << "\033[2J\033[1;1H";
	cout << "Num collide boxes: " << this->collideboxes.size() << "\n";
	
	for (list<CollideBox*>::iterator ito = this->collideboxes.begin(); ito != this->collideboxes.end(); ito++) {
		CollideBox *co = (*ito);
		
		list<CollideBox*>* tests = this->collides->getCollidesMC(co->x, co->y, co->radius);
		if (tests->size() < 2) {
			free(tests);
			continue;
		}
		
		cout << "Outer box: " << co << " at " << co->x << "x" << co->y << ":" << co->radius << "\n";
		
		for (list<CollideBox*>::iterator iti = tests->begin(); iti != tests->end(); iti++) {
			CollideBox *ci = (*iti);
			
			if (ci == co) continue;
			
			cout << "      box: " << ci << " at " << ci->x << "x" << ci->y << ":" << ci->radius;
			
			dist = sqrt(((ci->x - co->x) * (ci->x - co->x)) + ((ci->y - co->y) * (ci->y - co->y)));
			if (dist <= ci->radius + co->radius) {
				cout << "\t\tHIT";
				
				Event *ev = new Event();
				ev->type = ENTITY_HIT;
				ev->e1 = ci->e;
				ev->e2 = co->e;
				fireEvent(ev);
			}
			
			cout << "\n";
		}
		
		free(tests);
	}
	
	return;
	
	vector<Entity*>::iterator it1;
	vector<Entity*>::iterator it2;
	list<Entity*>::iterator it3;
	map<Entity*,Entity*>::iterator it4;
	map<Entity*,Entity*> rem;
	
	for (it1 = this->entities.begin(); it1 != this->entities.end(); it1++) {
		Entity *e1 = (*it1);
		if (! e1->collide) continue;
		if (e1->speed == 0.0) continue;
		if (rem.find(e1) != rem.end()) continue;
		
		// Have we hit anything
		for (it2 = it1; it2 != this->entities.end(); it2++) {
			Entity *e2 = (*it2);
			if (e2 == e1) continue;
			if (! e2->collide) continue;
			
			dist = sqrt(((e1->x - e2->x) * (e1->x - e2->x)) + ((e1->y - e2->y) * (e1->y - e2->y)));
			
			if (dist <= e1->radius + e2->radius) {
				if (find(e1->hits.begin(), e1->hits.end(), e2) == e1->hits.end()) {
					e1->hasHit(e2);
					e2->hasHit(e1);
					
					Event *ev = new Event();
					ev->type = ENTITY_HIT;
					ev->e1 = e1;
					ev->e2 = e2;
					fireEvent(ev);
				}
			}
		}
		
		// Have we UnHit anything?
		for (it3 = e1->hits.begin(); it3 != e1->hits.end(); it3++) {
			Entity *e2 = (*it3);
			
			dist = sqrt(((e1->x - e2->x) * (e1->x - e2->x)) + ((e1->y - e2->y) * (e1->y - e2->y)));
			if (dist > e1->radius + e2->radius) {
				rem[e1] = e2;
				rem[e2] = e1;
				
				Event *ev = new Event();
				ev->type = ENTITY_UNHIT;
				ev->e1 = e1;
				ev->e2 = e2;
				fireEvent(ev);
			}
		}
	}
	
	// Remove hits which should be removed
	for (it4 = rem.begin(); it4 != rem.end(); it4++) {
		(*it4).first->hits.remove((*it4).second);
	}
}


/**
* Returns an AreaType if we hit a wall, or NULL if we haven't
*
* Makes use of the AreaType check radius, as well as the checking objects'
* check radius.
*
* @todo Think about how this works with the new collission stuff,
*       Rehash away if nessasary
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
* @todo Think about how this works with the new collission stuff,
*       Rehash away if nessasary
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
* Only run this if you are in en 'update' method for an entity
**/
void GameState::addCollideBox(int x, int y, int radius, Entity *e)
{
	MapGridCell* cell = this->collides->getCellMC(x, y);
	
	CollideBox * box = new CollideBox(x, y, radius, e);
	
	cell->collideboxes.push_back(box);
	this->collideboxes.push_back(box);
}

