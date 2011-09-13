// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include <string>
#include "rage.h"

using namespace std;


/**
* Config file opts
**/
// Settings section
static cfg_opt_t unitsettings_opts[] =
{
	CFG_INT((char*) "max_speed", 0, CFGF_NONE),
	CFG_INT((char*) "accel", 0, CFGF_NONE),
	CFG_INT((char*) "turn_move", 0, CFGF_NONE),
	CFG_INT((char*) "turn_aim", 0, CFGF_NONE),
	CFG_END()
};

// State section
static cfg_opt_t unitstate_opts[] =
{
	CFG_STR((char*) "model", 0, CFGF_NONE),
	CFG_INT((char*) "type", 0, CFGF_NONE),
	CFG_END()
};

// Sound section
static cfg_opt_t unitsound_opts[] =
{
	CFG_STR((char*) "sound", 0, CFGF_NONE),
	CFG_INT((char*) "type", 0, CFGF_NONE),
	CFG_END()
};

// Unitclass section
cfg_opt_t unittype_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_SEC((char*) "settings", unitsettings_opts, CFGF_MULTI),
	CFG_SEC((char*) "state", unitstate_opts, CFGF_MULTI),
	CFG_SEC((char*) "sound", unitsound_opts, CFGF_MULTI),
	CFG_INT((char*) "playable", 1, CFGF_NONE),
	
	CFG_INT((char*) "begin_health", 0, CFGF_NONE),
	CFG_INT((char*) "hit_generator", 0, CFGF_NONE),
	
	CFG_STR_LIST((char*) "spawn_weapons", 0, CFGF_NONE),

	CFG_END()
};


/**
* Item loading function handler
**/
UnitType* loadItemUnitType(cfg_t* cfg_item, Mod* mod)
{
	UnitType* uc;
	cfg_t *cfg_settings, *cfg_state, *cfg_sound;
	int j;
	
	
	uc = new UnitType();
	uc->mod = mod;
	uc->name = cfg_getstr(cfg_item, "name");
	uc->begin_health = cfg_getint(cfg_item, "begin_health");
	uc->hit_generator = mod->getParticleGenType(cfg_getint(cfg_item, "hit_generator"));
	uc->playable = cfg_getint(cfg_item, "playable");
	
	
	/// Settings ///
	int num_settings = cfg_size(cfg_item, "settings");
	if (num_settings - 1 != UNIT_NUM_MODIFIERS) return NULL;
	
	// Load initial config
	cfg_settings = cfg_getnsec(cfg_item, "settings", 0);
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
		cfg_settings = cfg_getnsec(cfg_item, "settings", j);
		
		uc->modifiers[j - 1].max_speed = cfg_getint(cfg_settings, "max_speed");
		uc->modifiers[j - 1].accel = cfg_getint(cfg_settings, "accel");
		uc->modifiers[j - 1].turn_move = cfg_getint(cfg_settings, "turn_move");
		uc->modifiers[j - 1].turn_aim = cfg_getint(cfg_settings, "turn_aim");
	}
	
	
	/// States ///
	int num_states = cfg_size(cfg_item, "state");
	if (num_states < 1) return NULL;
	
	// load states
	uc->max_frames = 0;
	for (j = 0; j < num_states; j++) {
		cfg_state = cfg_getnsec(cfg_item, "state", j);
		
		UnitTypeState* uct = new UnitTypeState();
		
		uct->type = cfg_getint(cfg_state, "type");
		
		char * tmp = cfg_getstr(cfg_state, "model");
		if (tmp == NULL) return NULL;
		uct->model = mod->getAnimModel(tmp);
		
		uc->states.push_back(uct);
		uct->id = uc->states.size() - 1;
	}
	

	/// Sounds ///
	int num_sounds = cfg_size(cfg_item, "sound");
	
	// load sounds
	for (j = 0; j < num_sounds; j++) {
		cfg_sound = cfg_getnsec(cfg_item, "sound", j);
		
		UnitTypeSound* uts = new UnitTypeSound();
		
		uts->type = cfg_getint(cfg_sound, "type");
		
		char * tmp = cfg_getstr(cfg_sound, "sound");
		if (tmp == NULL) return NULL;
		uts->snd = mod->getSound(tmp);
		
		uc->sounds.push_back(uts);
	}


	/// Spawn Weapons ///
	int num_weapons = cfg_size(cfg_item, "spawn_weapons");

	for (j = 0; j < num_weapons; j++) {
		WeaponType * wt = mod->getWeaponType(cfg_getnstr(cfg_item, "spawn_weapons", j));
		
		if (wt != NULL) {
			uc->spawn_weapons.push_back(wt);
		}
	}


	return uc;
}



/**
* Returns the settings to use for any given set of modifier flags.
* The returned object should be freed by the caller.
**/
UnitTypeSettings* UnitType::getSettings(Uint8 modifier_flags)
{
	UnitTypeSettings *ret;
	
	ret = new UnitTypeSettings();
	
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
UnitTypeState* UnitType::getState(int type)
{
	unsigned int j = 0;
	unsigned int num = 0;
	
	// Find out how many of this time exist
	for (j = 0; j < this->states.size(); j++) {
		if (this->states.at(j)->type == type) num++;
	}
	
	// Randomly choose one
	num = getRandom(0, num - 1);
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
		reportFatalError("Cannot find state of type UNIT_STATE_STATIC for unit type " + this->name);
	}
	
	return this->getState(UNIT_STATE_STATIC);
}


/**
* Returns a random sound which matches the specified type.
* If it can't find a sound for that type, return NULL
**/
Sound* UnitType::getSound(int type)
{
	unsigned int j = 0;
	unsigned int num = 0;
	
	// Find out how many of this time exist
	for (j = 0; j < this->sounds.size(); j++) {
		if (this->sounds.at(j)->type == type) num++;
	}
	
	// Randomly choose one
	num = getRandom(0, num - 1);
	for (j = 0; j < this->sounds.size(); j++) {
		if (this->sounds.at(j)->type == type) {
			if (num == 0) {
				return this->sounds.at(j)->snd;
			}
			num--;
		}
	}
	
	return NULL;
}



UnitType::UnitType()
{
	this->width = 20;
	this->height = 20;
}

UnitType::~UnitType()
{
}
