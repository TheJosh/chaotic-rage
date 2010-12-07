// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include "rage.h"

using namespace std;


/* Variables */
extern cfg_opt_t g_action_opts;

/* Config file definition */
// Settings section
static cfg_opt_t settings_opts[] =
{
	CFG_INT((char*) "max_speed", 0, CFGF_NONE),
	CFG_INT((char*) "accel", 0, CFGF_NONE),
	CFG_INT((char*) "turn_move", 0, CFGF_NONE),
	CFG_INT((char*) "turn_aim", 0, CFGF_NONE),
	CFG_END()
};

// State section
static cfg_opt_t state_opts[] =
{
	CFG_STR((char*) "model", 0, CFGF_NONE),
	CFG_INT((char*) "type", 0, CFGF_NONE),
	CFG_END()
};

// Unitclass section
static cfg_opt_t unitclass_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_SEC((char*) "settings", settings_opts, CFGF_MULTI),
	CFG_SEC((char*) "state", state_opts, CFGF_MULTI),
	
	CFG_SEC((char*) "action", &g_action_opts, CFGF_MULTI),
	
	CFG_END()
};

// Main config
static cfg_opt_t opts[] =
{
	CFG_SEC((char*) "unitclass", unitclass_opts, CFGF_MULTI),
	CFG_END()
};



UnitClass::UnitClass() : EntityType()
{
	this->width = 20;
	this->height = 20;
}

UnitClass::~UnitClass()
{
}


/**
* Loads the area types
**/
vector<UnitClass*> * loadAllUnitClasses(Mod * mod)
{
	vector<UnitClass*> * unitclasses = new vector<UnitClass*>();
	
	char *buffer;
	cfg_t *cfg, *cfg_unitclass;
	
	
	// Load + parse the config file
	buffer = mod->loadText("unitclass/unitclass.conf");
	if (buffer == NULL) {
		return NULL;
	}
	
	cfg = cfg_init(opts, CFGF_NONE);
	cfg_parse_buf(cfg, buffer);
	
	free(buffer);
	
	
	int num_types = cfg_size(cfg, "unitclass");
	if (num_types == 0) return NULL;
	
	// Process area type sections
	int j;
	for (j = 0; j < num_types; j++) {
		cfg_unitclass = cfg_getnsec(cfg, "unitclass", j);
		
		UnitClass* uc = loadUnitClass(cfg_unitclass, mod);
		if (uc == NULL) {
			cerr << "Bad unit class at index " << j << endl;
			return false;
		}
		
		unitclasses->push_back(uc);
		uc->id = unitclasses->size() - 1;
	}
	
	// If there was sprite errors, exit the game
	if (mod->st->render->wasLoadSpriteError()) {
		cerr << "Error loading unit classes; game will now exit.\n";
		exit(1);
	}
	
	
	return unitclasses;
}



/**
* Loads a single area type
**/
UnitClass* loadUnitClass(cfg_t *cfg, Mod * mod)
{
	UnitClass* uc;
	cfg_t *cfg_settings, *cfg_state;
	int j;
	
	
	uc = new UnitClass();
	uc->name = cfg_getstr(cfg, "name");
	uc->mod = mod;
	
	uc->actions = loadActions(cfg);
	
	
	/// Settings ///
	int num_settings = cfg_size(cfg, "settings");
	if (num_settings - 1 != UNIT_NUM_MODIFIERS) return NULL;
	
	// Load initial config
	cfg_settings = cfg_getnsec(cfg, "settings", 0);
	uc->initial.max_speed = cfg_getint(cfg_settings, "max_speed");
	uc->initial.accel = cfg_getint(cfg_settings, "accel");
	uc->initial.turn_move = cfg_getint(cfg_settings, "turn_move");
	uc->initial.turn_aim = cfg_getint(cfg_settings, "turn_aim");
	
	if (uc->initial.max_speed == 0) return NULL;
	if (uc->initial.accel == 0) return NULL;
	if (uc->initial.turn_move == 0) return NULL;
	if (uc->initial.turn_aim == 0) return NULL;
	
	// load modifiers
	for (j = 1; j < num_settings; j++) {
		cfg_settings = cfg_getnsec(cfg, "settings", j);
		
		uc->modifiers[j - 1].max_speed = cfg_getint(cfg_settings, "max_speed");
		uc->modifiers[j - 1].accel = cfg_getint(cfg_settings, "accel");
		uc->modifiers[j - 1].turn_move = cfg_getint(cfg_settings, "turn_move");
		uc->modifiers[j - 1].turn_aim = cfg_getint(cfg_settings, "turn_aim");
	}
	
	
	
	/// States ///
	int num_states = cfg_size(cfg, "state");
	if (num_states < 1) return NULL;
	
	// load states
	uc->max_frames = 0;
	for (j = 0; j < num_states; j++) {
		cfg_state = cfg_getnsec(cfg, "state", j);
		
		UnitClassState* uct = new UnitClassState();
		
		uct->type = cfg_getint(cfg_state, "type");
		
		char * tmp = cfg_getstr(cfg_state, "model");
		if (tmp == NULL) return NULL;
		uct->model = mod->getAnimModel(tmp);
		
		uc->states.push_back(uct);
		uct->id = uc->states.size() - 1;
	}
	
	return uc;
}



/**
* Returns the settings to use for any given set of modifier flags.
* The returned object should be freed by the caller.
**/
UnitClassSettings* UnitClass::getSettings(Uint8 modifier_flags)
{
	UnitClassSettings *ret;
	
	ret = new UnitClassSettings();
	
	ret->max_speed = this->initial.max_speed;
	ret->accel = this->initial.accel;
	ret->turn_move = this->initial.turn_move;
	ret->turn_aim = this->initial.turn_aim;
	
	for (int i = 0; i < UNIT_NUM_MODIFIERS; i++) {
		if ((modifier_flags & (1 << i)) != 0) {
			ret->max_speed += this->modifiers[i - 1].max_speed;
			ret->accel += this->modifiers[i - 1].accel;
			ret->turn_move += this->modifiers[i - 1].turn_move;
			ret->turn_aim += this->modifiers[i - 1].turn_aim;
		}
	}
	
	return ret;
}


/**
* Returns a random state which matches the specified type.
* If no state for the specified type is found, uses a state from the UNIT_STATE_STATIC type.
**/
UnitClassState* UnitClass::getState(int type)
{
	unsigned int j = 0;
	unsigned int num = 0;
	
	// Find out how many of this time exist
	for (j = 0; j < this->states.size(); j++) {
		if (this->states.at(j)->type == type) num++;
	}
	
	// Randomly choose one
	num = getRandom(0, num);
	for (j = 0; j < this->states.size(); j++) {
		if (this->states.at(j)->type == type) {
			if (num == 0) {
				return this->states.at(j);
			}
			num--;
		}
	}
	
	// If no state of this type found, do a search for a static type
	if (type == UNIT_STATE_STATIC) {
		cerr << "Cannot find state of type UNIT_STATE_STATIC for unit.\n";
		exit(1);
	}
	
	return this->getState(UNIT_STATE_STATIC);
}


