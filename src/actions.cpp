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
	int j;
	
	actions = new vector<Action*>();
	
	int num_actions = cfg_size(cfg, "action");
	for (j = 0; j < num_actions; j++) {
		cfg_action = cfg_getnsec(cfg, "action", j);
		
		ac = new Action();
		ac->event = (ActionEvent) cfg_getint(cfg_action, "event");
		ac->type = (ActionType) cfg_getint(cfg_action, "type");
		
		actions->push_back(ac);
	}
	
	return actions;
}

