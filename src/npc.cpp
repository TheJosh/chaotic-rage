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
* Uses the currently pressed keys to change the player movement
**/
void NPC::update(int delta)
{
	UnitClassSettings *ucs = this->uc->getSettings(0);
	
	
	// This should be moved into a function in the GameLogic class
	if (st->game_time - vals[0] > 2500) {
		vals[0] = st->game_time;
		vals[1] = getAngleBetweenPoints(this->x, this->y, this->st->curr_player->x, this->st->curr_player->y);
		vals[1] = getRandom(vals[1] - 50, vals[1] + 50);
		
		this->desired_angle_aim = getRandom(vals[1] - 50, vals[1] + 50);
	}
	
	this->desired_angle_move = getRandom(vals[1] - 10, vals[1] + 10);
	
	this->speed += ppsDelta(ucs->accel, delta);
	
	if (this->inContactWith(WALL)) {
		this->speed = 0;
		this->setState(UNIT_STATE_STATIC);
	}
	// End of stuff to move
	
	
	Unit::update(delta, ucs);
	
	delete ucs;
}


