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
	
	this->current_state_type = 0;
	this->updateState(UNIT_STATE_STATIC);
	
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


void Unit::updateState(int new_type)
{
	if (this->current_state_type == new_type) return;
	
	this->current_state_type = new_type;
	this->current_state = this->uc->getState(new_type);
	this->animation_start = this->st->anim_frame;
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
	
	
	this->x = pointPlusAngleX(this->x, this->angle, ppsDelta(this->speed, delta));
	this->y = pointPlusAngleY(this->y, this->angle, ppsDelta(this->speed, delta));
}


