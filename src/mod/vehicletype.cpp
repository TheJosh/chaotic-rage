// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <confuse.h>
#include <btBulletDynamicsCommon.h>
#include "../rage.h"
#include "../render_opengl/assimpmodel.h"
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

// move nodes
static cfg_opt_t node_opts[] =
{
	CFG_STR((char*) "name", (char*)"", CFGF_NONE),
	CFG_FLOAT_LIST((char*) "axis", (char*)"{0.0, 0.0, 0.0}", CFGF_NONE),
	CFG_INT((char*) "type", 0, CFGF_NONE),
	CFG_END()
};

// Walltype section
cfg_opt_t vehicletype_opts[] =
{
	CFG_STR((char*) "name", (char*)"", CFGF_NONE),
	CFG_STR((char*) "model", (char*)"", CFGF_NONE),
	CFG_INT((char*) "health", 5000, CFGF_NONE),
	CFG_FLOAT((char*) "mass", 100.0f, CFGF_NONE),

	CFG_INT((char*) "land", 1, CFGF_NONE),
	CFG_INT((char*) "water", 0, CFGF_NONE),
	CFG_INT((char*) "helicopter", 0, CFGF_NONE),
	CFG_INT((char*) "engine", 1, CFGF_NONE),
	CFG_INT((char*) "steer", 1, CFGF_NONE),
	
	CFG_FLOAT((char*) "engine-accel", 20.0f, CFGF_NONE),
	CFG_FLOAT((char*) "engine-max", 200.0f, CFGF_NONE),
	CFG_FLOAT((char*) "brake-accel", 10.0f, CFGF_NONE),
	CFG_FLOAT((char*) "brake-max", 50.0f, CFGF_NONE),
	CFG_FLOAT((char*) "reverse-accel", 5.0f, CFGF_NONE),
	CFG_FLOAT((char*) "reverse-max", 30.0f, CFGF_NONE),
	
	CFG_STR((char*) "weapon-primary", (char*)"", CFGF_NONE),
	
	CFG_SEC((char*) "damage", damage_opts, CFGF_MULTI),
	CFG_SEC((char*) "node", node_opts, CFGF_MULTI),

	CFG_FLOAT_LIST((char*) "joint-front", (char*)"{0.0, 0.0, 0.0}", CFGF_NONE),
	CFG_FLOAT_LIST((char*) "joint-back", (char*)"{0.0, 0.0, 0.0}", CFGF_NONE),
	
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
	VehicleTypeNode node;

	wt = new VehicleType();
	wt->name = cfg_getstr(cfg_item, "name");
	wt->health = cfg_getint(cfg_item, "health");
	wt->mass = cfg_getfloat(cfg_item, "mass");
	wt->land = (cfg_getint(cfg_item, "land") == 1);
	wt->water = (cfg_getint(cfg_item, "water") == 1);
	wt->helicopter = (cfg_getint(cfg_item, "helicopter") == 1);
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
			mod->setLoadErr("Invalid model: %s", tmp);
			return NULL;
		}
		wt->col_shape = wt->model->getCollisionShape();
	}
	
	// Load move node, if set
	size = cfg_size(cfg_item, "node");
	for (int j = 0; j < size; j++) {
		cfg_t *cfg_node = cfg_getnsec(cfg_item, "node", j);
		
		tmp = cfg_getstr(cfg_node, "name");
		if (tmp[0] == '\0') continue;
		
		node.node = wt->model->findNode(tmp);
		if (node.node == NULL) {
			mod->setLoadErr("Node '%s' not found in model '%s'", tmp, wt->model->getName().c_str());
			return NULL;
		}

		node.type = (VehicleNodeType) cfg_getint(cfg_node, "type");
		node.axis = cfg_getvec3(cfg_node, "axis");
		
		wt->nodes.push_back(node);
	}
	
	// Load primary weapon, if set
	tmp = cfg_getstr(cfg_item, "weapon-primary");
	if (tmp != NULL && strlen(tmp) != 0) {
		wt->weapon_primary = mod->getWeaponType(tmp);
		if (! wt->weapon_primary) {
			mod->setLoadErr("Invalid primary weapon: %s", tmp);
			return NULL;
		}
	}
	
	// Load joint locations
	wt->joint_front = cfg_getvec3(cfg_item, "joint-front");
	wt->joint_back = cfg_getvec3(cfg_item, "joint-back");
	
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


/**
* Return true if the vehicle type has at least one node registered of that type
**/
bool VehicleType::hasNode(VehicleNodeType type)
{
	vector <VehicleTypeNode>::iterator it;
	for (it = this->nodes.begin(); it != this->nodes.end(); it++) {
		if ((*it).type == type) return true;
	}
	return false;
}


/**
* Return true if the vehicle type has at least one node registered of that type
**/
VehicleTypeNode* VehicleType::getNode(VehicleNodeType type)
{
	vector <VehicleTypeNode>::iterator it;
	for (it = this->nodes.begin(); it != this->nodes.end(); it++) {
		if ((*it).type == type) return &(*it);
	}
	return NULL;
}

