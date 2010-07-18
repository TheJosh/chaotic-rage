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
* Types of actions
* Such as create particle generator
**/
enum ActionType {
	ADD_PGENERATOR   = 1,
};

#define MAX_ACTION_ARGS 1

class Action {
	public:
		EventType event;
		ActionType type;
		int args[MAX_ACTION_ARGS];
};


vector<Action*>* loadActions(cfg_t *cfg);
