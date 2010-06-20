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
	HIT_WALL    = 1,  // particles, units, items
	HIT_UNIT    = 2,  // particles, units, items
	DEATH_AGE   = 3,  // particles, items
	LOSE_HP     = 4,  // units, items
	DEATH_HP    = 5,  // units, items
	ACTIVATE    = 6,  // items
};

/**
* Types of actions
* Such as create particle generator
**/
enum ActionType {
	ADD_PGENERATOR   = 1,
	ADD_PARTICLE     = 2,
	CHANGE_HP        = 3,
	REMOVE           = 4,
};

#define MAX_ACTION_ARGS 1

class Action {
	public:
		ActionEvent event;
		ActionType type;
		int args[MAX_ACTION_ARGS];
};


vector<Action*>* loadActions(cfg_t *cfg);
