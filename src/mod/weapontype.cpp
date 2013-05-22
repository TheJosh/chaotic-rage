// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include "../rage.h"

using namespace std;


// Sound section
static cfg_opt_t weaponsound_opts[] =
{
	CFG_STR((char*) "sound", 0, CFGF_NONE),
	CFG_INT((char*) "type", 0, CFGF_NONE),
	CFG_END()
};

/**
* Config file opts
**/
cfg_opt_t weapontype_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_STR((char*) "title", 0, CFGF_NONE),
	CFG_INT((char*) "type", 1, CFGF_NONE),
	CFG_INT((char*) "fire_delay", 0, CFGF_NONE),
	CFG_INT((char*) "reload_delay", 0, CFGF_NONE),
	CFG_INT((char*) "continuous", 0, CFGF_NONE),
	CFG_INT((char*) "magazine_limit", 100, CFGF_NONE),
	CFG_INT((char*) "belt_limit", 1000, CFGF_NONE),

	// WEAPON_TYPE_RAYCAST
	// WEAPON_TYPE_FLAMETHROWER
	CFG_INT((char*) "angle_range", 0, CFGF_NONE),
	CFG_FLOAT((char*) "range", 50, CFGF_NONE),
	CFG_FLOAT((char*) "unit_damage", 10, CFGF_NONE),
	CFG_FLOAT((char*) "wall_damage", 10, CFGF_NONE),
	
	// WEAPON_TYPE_DIGDOWN
	CFG_INT((char*) "radius", 2, CFGF_NONE),
	CFG_FLOAT((char*) "depth", -0.1, CFGF_NONE),
	
	// WEAPON_TYPE_TIMED_MINE
	CFG_INT((char*) "time", 1000, CFGF_NONE),
	CFG_STR((char*) "model", 0, CFGF_NONE),

	CFG_SEC((char*) "sound", weaponsound_opts, CFGF_MULTI),
	CFG_END()
};


/**
* Item loading function handler
**/
WeaponType* loadItemWeaponType(cfg_t* cfg_item, Mod* mod)
{
	WeaponType* wt;
	cfg_t *cfg_sound;
	string filename;
	int j, type;
	
	type = cfg_getint(cfg_item, "type");

	// The exact class is dependent on the type
	if (type == WEAPON_TYPE_RAYCAST) {
		WeaponRaycast* w = new WeaponRaycast();
		w->angle_range = cfg_getint(cfg_item, "angle_range");
		w->range = cfg_getfloat(cfg_item, "range");
		w->unit_damage = cfg_getfloat(cfg_item, "unit_damage");		// TODO: merge unit/wall damage
		w->wall_damage = cfg_getfloat(cfg_item, "wall_damage");		// TODO: merge unit/wall damage
		wt = w;

	} else if (type == WEAPON_TYPE_DIGDOWN) {
		WeaponDigdown* w = new WeaponDigdown();
		w->radius = cfg_getint(cfg_item, "radius");
		w->depth = cfg_getfloat(cfg_item, "depth");
		wt = w;

	} else if (type == WEAPON_TYPE_FLAMETHROWER) {
		WeaponFlamethrower* w = new WeaponFlamethrower();
		w->angle_range = cfg_getint(cfg_item, "angle_range");
		w->range = cfg_getfloat(cfg_item, "range");
		w->unit_damage = cfg_getfloat(cfg_item, "unit_damage");		// TODO: merge unit/wall damage
		w->wall_damage = cfg_getfloat(cfg_item, "wall_damage");		// TODO: merge unit/wall damage
		wt = w;

	} else if (type == WEAPON_TYPE_TIMED_MINE) {
		WeaponTimedMine* w = new WeaponTimedMine();
		w->time = cfg_getint(cfg_item, "time");

		char* tmp = cfg_getstr(cfg_item, "model");
		if (tmp) w->model = mod->getAssimpModel(tmp);

		w->range = cfg_getfloat(cfg_item, "range");
		w->damage = cfg_getfloat(cfg_item, "unit_damage");		// TODO: merge unit/wall damage

		wt = w;

	} else if (type == WEAPON_TYPE_PROXI_MINE) {
		WeaponProxiMine* w = new WeaponProxiMine();

		char* tmp = cfg_getstr(cfg_item, "model");
		if (tmp) w->model = mod->getAssimpModel(tmp);

		w->range = cfg_getfloat(cfg_item, "range");
		w->damage = cfg_getfloat(cfg_item, "unit_damage");		// TODO: merge unit/wall damage

		wt = w;

	} else if (type == WEAPON_TYPE_REMOTE_MINE) {
		WeaponRemoteMine* w = new WeaponRemoteMine();

		char* tmp = cfg_getstr(cfg_item, "model");
		if (tmp) w->model = mod->getAssimpModel(tmp);

		w->range = cfg_getfloat(cfg_item, "range");
		w->damage = cfg_getfloat(cfg_item, "unit_damage");		// TODO: merge unit/wall damage

		wt = w;

	} else if (type == WEAPON_TYPE_REMOTE_TRIG) {
		WeaponRemoteTrigger* w = new WeaponRemoteTrigger();
		wt = w;

	} else if (type == WEAPON_TYPE_ROCKET) {
		WeaponRocket* w = new WeaponRocket();
		wt = w;
		
		char* tmp = cfg_getstr(cfg_item, "model");
		if (tmp) w->model = mod->getAssimpModel(tmp);
		
		// TODO: blast radius vs detection radius?
		w->range = cfg_getfloat(cfg_item, "range");
		w->damage = cfg_getfloat(cfg_item, "unit_damage");		// TODO: merge unit/wall damage
		
		
	} else {
		return NULL;
	}


	// These settings are common to all weapon types
	wt->name = cfg_getstr(cfg_item, "name");
	wt->title = cfg_getstr(cfg_item, "title");
	wt->st = mod->st;
	wt->type = type;

	wt->fire_delay = cfg_getint(cfg_item, "fire_delay");
	wt->reload_delay = cfg_getint(cfg_item, "reload_delay");
	wt->continuous = (cfg_getint(cfg_item, "continuous") == 1);
	wt->magazine_limit = cfg_getint(cfg_item, "magazine_limit");
	wt->belt_limit = cfg_getint(cfg_item, "belt_limit");
	
	// Load sounds
	int num_sounds = cfg_size(cfg_item, "sound");
	for (j = 0; j < num_sounds; j++) {
		cfg_sound = cfg_getnsec(cfg_item, "sound", j);
		
		WeaponTypeSound* wts = new WeaponTypeSound();
		wts->type = cfg_getint(cfg_sound, "type");
		
		char * tmp = cfg_getstr(cfg_sound, "sound");
		if (tmp == NULL) return NULL;
		wts->snd = mod->getSound(tmp);
		
		wt->sounds.push_back(wts);
	}
	
	return wt;
}


/**
* Returns a random sound which matches the specified type.
* If it can't find a sound for that type, return NULL
**/
Sound* WeaponType::getSound(int type)
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


WeaponType::WeaponType()
{
}

