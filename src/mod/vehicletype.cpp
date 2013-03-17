// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include "../rage.h"

using namespace std;


/**
* Config file opts
**/
// Damage
static cfg_opt_t damage_opts[] =
{
	CFG_INT((char*) "health", 0, CFGF_NONE),
	CFG_STR((char*) "model", (char*)"", CFGF_NONE),
	CFG_END()
};

// Walltype section
cfg_opt_t vehicletype_opts[] =
{
	CFG_STR((char*) "name", (char*)"", CFGF_NONE),
	CFG_STR((char*) "model", (char*)"", CFGF_NONE),
	CFG_INT((char*) "health", 5000, CFGF_NONE),
	CFG_INT((char*) "land", 1, CFGF_NONE),
	CFG_INT((char*) "water", 0, CFGF_NONE),
	
	CFG_FLOAT((char*) "engine-accel", 20.0f, CFGF_NONE),
	CFG_FLOAT((char*) "engine-max", 200.0f, CFGF_NONE),
	CFG_FLOAT((char*) "brake-accel", 10.0f, CFGF_NONE),
	CFG_FLOAT((char*) "brake-max", 50.0f, CFGF_NONE),
	CFG_FLOAT((char*) "reverse-accel", 5.0f, CFGF_NONE),
	CFG_FLOAT((char*) "reverse-max", 30.0f, CFGF_NONE),
	
	CFG_SEC((char*) "damage", damage_opts, CFGF_MULTI),
	
	CFG_END()
};


/**
* Item loading function handler
**/
VehicleType* loadItemVehicleType(cfg_t* cfg_item, Mod* mod)
{
	VehicleType* wt;
	string filename;
	int size;
	
	wt = new VehicleType();
	wt->name = cfg_getstr(cfg_item, "name");
	wt->health = cfg_getint(cfg_item, "health");
	wt->land = cfg_getint(cfg_item, "land");
	wt->water = cfg_getint(cfg_item, "water");

	wt->engine_accel = cfg_getfloat(cfg_item, "engine-accel");
	wt->engine_max = cfg_getfloat(cfg_item, "engine-max");
	wt->brake_accel = cfg_getfloat(cfg_item, "brake-accel");
	wt->brake_max = cfg_getfloat(cfg_item, "brake-max");
	wt->reverse_accel = cfg_getfloat(cfg_item, "reverse-accel");
	wt->reverse_max = cfg_getfloat(cfg_item, "reverse-max");
	
	char * tmp = cfg_getstr(cfg_item, "model");
	if (tmp != NULL) {
		wt->model = mod->getAssimpModel(tmp);
		if (! wt->model) return NULL;
	}
	
	// Load damage states
	size = cfg_size(cfg_item, "damage");
	if (size == 0) {
		VehicleTypeDamage * dam = new VehicleTypeDamage();
		dam->health = 0;
		dam->model = mod->getAssimpModel("null.blend");
		wt->damage_models.push_back(dam);
		
	} else {
		for (int j = 0; j < size; j++) {
			cfg_t *cfg_damage = cfg_getnsec(cfg_item, "damage", j);
			
			char * tmp = cfg_getstr(cfg_damage, "model");
			if (tmp == NULL) return NULL;
			
			VehicleTypeDamage * dam = new VehicleTypeDamage();
			
			dam->health = cfg_getint(cfg_damage, "health");
			dam->model = mod->getAssimpModel(tmp);
			if (! dam->model) return NULL;
			
			wt->damage_models.push_back(dam);
		}
	}
	
	return wt;
}


VehicleType::VehicleType()
{
}
