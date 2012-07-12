// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"

using namespace std;


NPC::NPC(UnitType *uc, GameState *st, float x, float y, float z, AIType *ai) : Unit(uc, st, x, y, z)
{
	vals[0] = vals[1] = vals[2] = vals[3] = 0;
	
	this->setState(UNIT_STATE_RUNNING);
	
	logic = new AILogic(this);
	logic->execScript(ai->script);
}

NPC::~NPC()
{
}


void NPC::hasBeenHit(Entity * that)
{
	Unit::hasBeenHit(that);
}


/**
* Try to find a player and attack them
**/
void NPC::update(int delta)
{
	if (remove_at != 0) {
		if (remove_at <= st->game_time) this->del = 1;
		return;
	}
	
	UnitTypeSettings *ucs = this->uc->getSettings(0);
	
	logic->update(delta);
	
	Unit::update(delta, ucs);
	
	delete ucs;
}

int NPC::takeDamage(int damage)
{
	int result = Unit::takeDamage(damage);
	
	if (result == 1) {
		this->st->logic->raise_npcdied();

		int s = getRandom(1, 3);
		if (s == 1) {
			this->st->audio->playSound(this->st->mm->getSound("zombie_death1"), false, this);
		} else if (s == 2) {
			this->st->audio->playSound(this->st->mm->getSound("zombie_death2"), false, this);
		} else if (s == 3) {
			this->st->audio->playSound(this->st->mm->getSound("zombie_death3"), false, this);
		}
	}
	
	return result;
}
