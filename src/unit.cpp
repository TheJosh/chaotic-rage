#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"

using namespace std;


Unit::Unit(UnitClass *uc, GameState *st) : Entity(st)
{
	this->uc = uc;
	this->angle = 0;
	this->desired_angle = 0;
	this->speed = 0;
	this->health = 100;
	
	this->weapon = NULL;
	this->weapon_gen = NULL;
	this->firing = false;
	
	this->current_state_type = 0;
	this->setState(UNIT_STATE_STATIC);
	
	this->sprites = uc->loadAllSprites();
	if (wasLoadSpriteError()) {
		cerr << "Unable to load required unit sprites; exiting.\n";
		exit(1);
	}
}

Unit::~Unit()
{
	unsigned int j;
	for (j = 0; j < this->sprites->size(); j++) {
		SDL_FreeSurface(this->sprites->at(j));
	}
	
	delete(this->sprites);
}


void Unit::setState(int new_type)
{
	if (this->current_state_type == new_type) return;
	
	this->current_state_type = new_type;
	this->current_state = this->uc->getState(new_type);
	this->animation_start = this->st->anim_frame;
}


void Unit::setWeapon(WeaponType* wt)
{
	this->weapon = wt;
	this->firing = false;
	
	delete this->weapon_gen;
	this->weapon_gen = new ParticleGenerator(wt->pg, this->st);
}


void Unit::beginFiring()
{
	this->weapon_gen->age = 0;
	this->firing = true;
}

void Unit::endFiring()
{
	this->firing = false;
}


/**
* Moves units around
**/
void Unit::update(int delta, UnitClassSettings *ucs)
{
	int turn_speed = ppsDelta(ucs->turn_speed, delta);
	
	// Calculate clockwise and anti-clockwise difference
	int diff_anti = this->desired_angle;
	if (diff_anti < this->angle) diff_anti += 360;
	diff_anti -= this->angle;
	int diff_clock = 360 - diff_anti;
	
	// the shortest distance - go in that direction
	if (diff_anti < diff_clock) {
		this->angle += (turn_speed < diff_anti ? turn_speed : diff_anti);
	} else if (diff_clock < diff_anti) {
		this->angle -= (turn_speed < diff_clock ? turn_speed : diff_clock);
	}
	
	// clamp to legal values
	if (this->angle < 0) this->angle += 360;
	if (this->angle > 359) this->angle -= 360;
	
	
	// Move
	int newx = pointPlusAngleX(this->x, this->angle, ppsDelta(this->speed, delta));
	int newy = pointPlusAngleY(this->y, this->angle, ppsDelta(this->speed, delta));
	
	// Collision detection
	if (collideWall(this->st, newx, newy, this->uc->width, this->uc->height)) {
		this->speed = 0;
		this->setState(UNIT_STATE_STATIC);
		
	} else {
		this->x = newx;
		this->y = newy;
	}
	
	// Fire bullets
	if (this->firing) {
		this->weapon_gen->x = this->x;
		this->weapon_gen->y = this->y;
		
		this->weapon_gen->update(delta);
	}
}


