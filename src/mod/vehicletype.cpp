// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <confuse.h>
#include <zzip/zzip.h>
#include <btBulletDynamicsCommon.h>
#include "../rage.h"
#include "../render/assimpmodel.h"
#include "mod.h"
#include "vehicletype.h"
#include "confuse_types.h"


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
	CFG_INT((char*) "engine", 1, CFGF_NONE),
	CFG_INT((char*) "steer", 1, CFGF_NONE),
	
	CFG_FLOAT((char*) "engine-accel", 20.0f, CFGF_NONE),
	CFG_FLOAT((char*) "engine-max", 200.0f, CFGF_NONE),
	CFG_FLOAT((char*) "brake-accel", 10.0f, CFGF_NONE),
	CFG_FLOAT((char*) "brake-max", 50.0f, CFGF_NONE),
	CFG_FLOAT((char*) "reverse-accel", 5.0f, CFGF_NONE),
	CFG_FLOAT((char*) "reverse-max", 30.0f, CFGF_NONE),
	
	CFG_STR((char*) "horiz-move-node", (char*)"", CFGF_NONE),
	CFG_FLOAT_LIST((char*) "horiz-move-axis", 0, CFGF_NONE),
	
	CFG_STR((char*) "vert-move-node", (char*)"", CFGF_NONE),
	CFG_FLOAT_LIST((char*) "vert-move-axis", 0, CFGF_NONE),
	
	CFG_STR((char*) "weapon-primary", (char*)"", CFGF_NONE),
	
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
	char* tmp;
	
	wt = new VehicleType();
	wt->name = cfg_getstr(cfg_item, "name");
	wt->health = cfg_getint(cfg_item, "health");
	wt->land = (cfg_getint(cfg_item, "land") == 1);
	wt->water = (cfg_getint(cfg_item, "water") == 1);
	wt->engine = (cfg_getint(cfg_item, "engine") == 1);
	wt->steer = (cfg_getint(cfg_item, "steer") == 1);
	
	wt->engine_accel = cfg_getfloat(cfg_item, "engine-accel");
	wt->engine_max = cfg_getfloat(cfg_item, "engine-max");
	wt->brake_accel = cfg_getfloat(cfg_item, "brake-accel");
	wt->brake_max = cfg_getfloat(cfg_item, "brake-max");
	wt->reverse_accel = cfg_getfloat(cfg_item, "reverse-accel");
	wt->reverse_max = cfg_getfloat(cfg_item, "reverse-max");
	
	// Load model
	tmp = cfg_getstr(cfg_item, "model");
	if (tmp != NULL && strlen(tmp) != 0) {
		wt->model = mod->getAssimpModel(tmp);
		if (! wt->model) {
			cerr << "Invalid model: " << tmp << endl;
			return NULL;
		}
	}
	
	// Load the collision shape
	btVector3 sizeHE = wt->model->getBoundingSizeHE();
	wt->col_shape = new btBoxShape(sizeHE);
	
	// Load move node, if set
	tmp = cfg_getstr(cfg_item, "horiz-move-node");
	if (tmp != NULL) {
		wt->horiz_move_node = tmp;
		wt->horiz_move_axis = cfg_getvec3(cfg_item, "horiz-move-axis");
	}
	
	// Load move node, if set
	tmp = cfg_getstr(cfg_item, "vert-move-node");
	if (tmp != NULL) {
		wt->vert_move_node = tmp;
		wt->vert_move_axis = cfg_getvec3(cfg_item, "vert-move-axis");
	}
	
	// Load primary weapon, if set
	tmp = cfg_getstr(cfg_item, "weapon-primary");
	if (tmp != NULL && strlen(tmp) != 0) {
		wt->weapon_primary = mod->getWeaponType(tmp);
		if (! wt->weapon_primary) {
			cerr << "Invalid primary weapon: " << tmp << endl;
			return NULL;
		}
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
	this->weapon_primary = NULL;
	this->horiz_move_node = "";
	this->vert_move_node = "";
	this->col_shape = NULL;
	this->land = false;
	this->water = false;
	this->engine = false;
	this->steer = false;
}

VehicleType::~VehicleType()
{
	delete(this->col_shape);
}


