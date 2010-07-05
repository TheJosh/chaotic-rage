// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <vector>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include "rage.h"

using namespace std;


/* Config file definition */
// Areatype section
cfg_opt_t g_action_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_INT((char*) "event", 0, CFGF_NONE),
	CFG_INT((char*) "type", 0, CFGF_NONE),
	CFG_INT_LIST((char*) "args", 0, CFGF_NONE),
	CFG_END()
};


/**
* Loads the actions for an item
**/
vector<Action*>* loadActions(cfg_t *cfg)
{
	vector<Action*>* actions;
	Action* ac;
	cfg_t *cfg_action;
	int i, j;
	
	int num_actions = cfg_size(cfg, "action");
	if (num_actions == 0) return NULL;
	
	actions = new vector<Action*>();
	
	for (i = 0; i < num_actions; i++) {
		cfg_action = cfg_getnsec(cfg, "action", i);
		
		ac = new Action();
		ac->event = (ActionEvent) cfg_getint(cfg_action, "event");
		ac->type = (ActionType) cfg_getint(cfg_action, "type");
		
		int num_args = cfg_size(cfg_action, "args");
		if (num_args > MAX_ACTION_ARGS) {
			cerr << "Too number of argumens provided\n";
			exit(1);
		}
		
		for (j = 0; j < num_args; j++) {
			ac->args[j] = cfg_getnint(cfg_action, "args", j);
		}
		
		actions->push_back(ac);
	}
	
	return actions;
}

