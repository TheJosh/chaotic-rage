// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include "../rage.h"
#include "../game_state.h"
#include "../render_opengl/animplay.h"
#include "../mod/unittype.h"
#include "../mod/aitype.h"
#include "../lua/ailogic.h"
#include "../audio/audio.h"
#include "../mod/mod_manager.h"
#include  "../lua/gamelogic.h"
#include "npc.h"


using namespace std;


NPC::NPC(UnitType *uc, GameState *st, float x, float y, float z, AIType *ai) : Unit(uc, st, x, y, z)
{
	vals[0] = vals[1] = vals[2] = vals[3] = 0;
	
	this->logic = new AILogic(this);
	this->logic->execScript(ai->script);
	
	this->idle_sound_time = st->game_time + 15000;
}

NPC::~NPC()
{
	if (this->logic) {
		delete(this->logic);
	}
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

	logic->update(delta);
	Unit::update(delta);
	
	if (this->idle_sound_time < st->game_time) {
		int s = getRandom(1, 3);
		if (s == 1) {
			this->st->audio->playSound(this->st->mm->getSound("zombie_2"), false, this);
		} else if (s == 2) {
			this->st->audio->playSound(this->st->mm->getSound("zombie_3"), false, this);
		} else if (s == 3) {
			this->st->audio->playSound(this->st->mm->getSound("zombie_4"), false, this);
		}
		this->idle_sound_time = st->game_time + 15000;
	}
}

int NPC::takeDamage(float damage)
{
	int result = Unit::takeDamage(damage);
	
	if (result == 1) {
		this->st->logic->raise_npcdied();
		
		// Play a sound
		int s = getRandom(1, 3);
		if (s == 1) {
			this->st->audio->playSound(this->st->mm->getSound("zombie_death1"), false, this);
		} else if (s == 2) {
			this->st->audio->playSound(this->st->mm->getSound("zombie_death2"), false, this);
		} else if (s == 3) {
			this->st->audio->playSound(this->st->mm->getSound("zombie_death3"), false, this);
		}
		
		this->st->deadButNotBuried(this);
	}
	
	return result;
}
