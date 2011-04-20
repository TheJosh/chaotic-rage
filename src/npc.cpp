// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


NPC::NPC(UnitClass *uc, GameState *st) : Unit(uc, st)
{
	vals[0] = vals[1] = vals[2] = vals[3] = 0;
	
	this->setState(UNIT_STATE_RUNNING);
}

NPC::~NPC()
{
}


/**
* Hit player; attack!
**/
void NPC::handleEvent(Event * ev)
{
	if (ev->type == ENTITY_HIT) {
		
		
		// This should be moved into a function in the GameLogic class
		Entity *e = (ev->e1 == this ? ev->e2 : ev->e1);
		
		if (e->klass() == UNIT && ((Unit*)e)->fac != this->fac) {
			this->beginFiring();
			
		}
		// End of stuff to move
		
		
	} else if (ev->type == ENTITY_UNHIT) {
		Entity *e = (ev->e1 == this ? ev->e2 : ev->e1);
		
		if (e->klass() == UNIT) {
			this->endFiring();
		}
	}
	
	Unit::handleEvent(ev);
}


/**
* Try to find a player and attack them
**/
void NPC::update(int delta)
{
	UnitClassSettings *ucs = this->uc->getSettings(0);
	
	
	// This should be moved into a function in the GameLogic class
	if (st->curr_player == NULL) return;
	
	if (st->game_time - vals[0] > 2500) {
		vals[0] = st->game_time;
		vals[1] = getAngleBetweenPoints((int)round(this->x), (int)round(this->y), (int)round(this->st->curr_player->x), (int)round(this->st->curr_player->y));
		vals[1] = getRandom(vals[1] - 50, vals[1] + 50);
		
		this->desired_angle_aim = getRandom(vals[1] - 50, vals[1] + 50);
	}
	
	this->desired_angle_move = getRandom(vals[1] - 10, vals[1] + 10);
	
	this->speed += ppsDeltaf(ucs->accel, delta);
	// End of stuff to move
	
	
	Unit::update(delta, ucs);
	
	delete ucs;
}


