// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include "rage.h"

using namespace std;


/**
* Events which are processed by actions
* Such as hitting a wall
**/
enum ActionEvent {
	HIT_WALL       = 1,  // particles
	HIT_UNIT       = 2,  // particles
	HIT_PARTICLE   = 3,  // units
};

/**
* Types of actions
* Such as create particle generator
**/
enum ActionType {
	ADD_PGENERATOR   = 1,
};

#define MAX_ACTION_ARGS 1

class Action {
	public:
		ActionEvent event;
		ActionType type;
		int args[MAX_ACTION_ARGS];
};


vector<Action*>* loadActions(cfg_t *cfg);
