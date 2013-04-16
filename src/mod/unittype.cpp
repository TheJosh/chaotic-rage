// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include <string>
#include "../rage.h"

using namespace std;


/**
* Config file opts
**/
// Settings section
static cfg_opt_t unitsettings_opts[] =
{
	CFG_FLOAT((char*) "max_speed", 0.0f, CFGF_NONE),
	CFG_FLOAT((char*) "accel", 0.0f, CFGF_NONE),
	CFG_INT((char*) "turn", 0, CFGF_NONE),
	CFG_INT((char*) "melee_damage", 1000, CFGF_NONE),
	CFG_INT((char*) "melee_delay", 100, CFGF_NONE),
	CFG_INT((char*) "melee_cooldown", 100, CFGF_NONE),
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
	CFG_STR((char*) "model", 0, CFGF_NONE),
	
	CFG_SEC((char*) "settings", unitsettings_opts, CFGF_MULTI),
	CFG_SEC((char*) "sound", unitsound_opts, CFGF_MULTI),
	CFG_STR_LIST((char*) "spawn_weapons", 0, CFGF_NONE),

	CFG_INT((char*) "playable", 1, CFGF_NONE),
	CFG_INT((char*) "health", 0, CFGF_NONE),

	CFG_END()
};


/**
* Item loading function handler
**/
UnitType* loadItemUnitType(cfg_t* cfg_item, Mod* mod)
{
	UnitType* uc;
	cfg_t *cfg_settings, *cfg_sound;
	int j;
	
	
	uc = new UnitType();
	uc->mod = mod;
	uc->name = cfg_getstr(cfg_item, "name");
	uc->begin_health = cfg_getint(cfg_item, "health");
	uc->playable = cfg_getint(cfg_item, "playable");
	
	char * tmp = cfg_getstr(cfg_item, "model");
	if (tmp == NULL) return NULL;
	uc->model = mod->getAssimpModel(tmp);
	if (uc->model == NULL) return NULL;
	
	
	/// Settings ///
	int num_settings = cfg_size(cfg_item, "settings");
	if (num_settings - 1 != UNIT_NUM_MODIFIERS) return NULL;
	
	// Load initial config
	cfg_settings = cfg_getnsec(cfg_item, "settings", 0);
	uc->initial.max_speed = cfg_getfloat(cfg_settings, "max_speed");
	uc->initial.accel = cfg_getfloat(cfg_settings, "accel");
	uc->initial.turn = cfg_getint(cfg_settings, "turn");
	uc->initial.melee_damage = cfg_getint(cfg_settings, "melee_damage");
	uc->initial.melee_delay = cfg_getint(cfg_settings, "melee_delay");
	uc->initial.melee_cooldown = cfg_getint(cfg_settings, "melee_cooldown");

	if (uc->initial.max_speed == 0) return NULL;
	if (uc->initial.accel == 0) return NULL;
	if (uc->initial.turn == 0) return NULL;
	
	// load modifiers
	for (j = 1; j < num_settings; j++) {
		cfg_settings = cfg_getnsec(cfg_item, "settings", j);
		
		uc->modifiers[j - 1].max_speed = cfg_getfloat(cfg_settings, "max_speed");
		uc->modifiers[j - 1].accel = cfg_getfloat(cfg_settings, "accel");
		uc->modifiers[j - 1].turn = cfg_getint(cfg_settings, "turn");
		uc->modifiers[j - 1].melee_damage = cfg_getint(cfg_settings, "melee_damage");
		uc->modifiers[j - 1].melee_delay = cfg_getint(cfg_settings, "melee_delay");
		uc->modifiers[j - 1].melee_cooldown = cfg_getint(cfg_settings, "melee_cooldown");
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
	ret->turn = this->initial.turn;
	ret->melee_cooldown = this->initial.melee_cooldown;
	ret->melee_damage = this->initial.melee_damage;
	ret->melee_delay = this->initial.melee_delay;

	for (int i = 0; i < UNIT_NUM_MODIFIERS; i++) {
		if ((modifier_flags & (1 << i)) != 0) {
			ret->max_speed += this->modifiers[i - 1].max_speed;
			ret->accel += this->modifiers[i - 1].accel;
			ret->turn += this->modifiers[i - 1].turn;
			ret->melee_cooldown += this->modifiers[i - 1].melee_cooldown;
			ret->melee_damage += this->modifiers[i - 1].melee_damage;
			ret->melee_delay += this->modifiers[i - 1].melee_delay;
		}
	}
	
	return ret;
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
