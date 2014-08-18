// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "npc.h"
#include "../game_state.h"
#include "../lua/ailogic.h"
#include "../lua/gamelogic.h"
#include "../mod/aitype.h"
#include "../rage.h"
#include "unit.h"

class UnitType;


using namespace std;


NPC::NPC(UnitType *uc, GameState *st, float x, float y, float z, AIType *ai, Faction fac) : Unit(uc, st, x, y, z, fac)
{
	vals[0] = vals[1] = vals[2] = vals[3] = 0;

	this->logic = new AILogic(this);
	this->logic->execScript(ai->script);

	this->idle_sound_time = st->game_time + 15000;
}

NPC::~NPC()
{
	delete(this->logic);
	this->logic = NULL;
}


/**
* Try to find a player and attack them
**/
void NPC::update(int delta)
{
	logic->update();
	Unit::update(delta);

	if (this->idle_sound_time < st->game_time) {
		// TODO: play idle sound
		this->idle_sound_time = st->game_time + 15000;
	}
}

int NPC::takeDamage(float damage)
{
	int result = Unit::takeDamage(damage);

	if (result == 1) {
		this->st->logic->raise_npcdied();

		this->st->deadButNotBuried(this, this->anim);
	}

	return result;
}
