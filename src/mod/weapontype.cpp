// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <confuse.h>
#include <zzip/zzip.h>
#include "../rage.h"
#include "../util/quadratic.h"
#include "mod.h"
#include "weapontype.h"
#include "../weapons/weapons.h"


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
	
	// Sounds
	CFG_SEC((char*) "sound", weaponsound_opts, CFGF_MULTI),
	
	// The per-type settings fall into one of five categories:
	CFG_SEC((char*) "raycast", weapconf_raycast, CFGF_NONE),
	CFG_SEC((char*) "digdown", weapconf_digdown, CFGF_NONE),
	CFG_SEC((char*) "mine", weapconf_mine, CFGF_NONE),
	CFG_SEC((char*) "rocket", weapconf_rocket, CFGF_NONE),
	CFG_SEC((char*) "attractor", weapconf_attractor, CFGF_NONE),
	
	CFG_END()
};


/**
* Item loading function handler
**/
WeaponType* loadItemWeaponType(cfg_t* cfg_item, Mod* mod)
{
	WeaponType* wt;
	cfg_t *cfg_sound, *cfg_sec;
	string filename;
	int j, type;
	
	type = cfg_getint(cfg_item, "type");
	
	// The exact class is dependent on the type
	switch (type) {
		case WEAPON_TYPE_RAYCAST:
			{
				WeaponRaycast* w = new WeaponRaycast();
				cfg_sec = cfg_getnsec(cfg_item, "raycast", 0);
				wt = w;

				w->angle_range = cfg_getint(cfg_sec, "angle_range");
				w->range = cfg_getfloat(cfg_sec, "range");
				w->unit_damage = cfg_getfloat(cfg_sec, "unit_damage");		// TODO: merge unit/wall damage
				w->wall_damage = cfg_getfloat(cfg_sec, "wall_damage");		// TODO: merge unit/wall damage
			}
			break;

		case WEAPON_TYPE_DIGDOWN:
			{
				WeaponDigdown* w = new WeaponDigdown();
				cfg_sec = cfg_getnsec(cfg_item, "digdown", 0);
				wt = w;

				w->radius = cfg_getint(cfg_sec, "radius");
				w->depth = cfg_getfloat(cfg_sec, "depth");
			}
			break;

		case WEAPON_TYPE_FLAMETHROWER:
			{
				WeaponFlamethrower* w = new WeaponFlamethrower();
				cfg_sec = cfg_getnsec(cfg_item, "raycast", 0);
				wt = w;

				w->angle_range = cfg_getint(cfg_sec, "angle_range");
				w->range = cfg_getfloat(cfg_sec, "range");
				w->unit_damage = cfg_getfloat(cfg_sec, "unit_damage");		// TODO: merge unit/wall damage
				w->wall_damage = cfg_getfloat(cfg_sec, "wall_damage");		// TODO: merge unit/wall damage
			}
			break;

		case WEAPON_TYPE_TIMED_MINE:
			{
				WeaponTimedMine* w = new WeaponTimedMine();
				cfg_sec = cfg_getnsec(cfg_item, "mine", 0);
				wt = w;

				w->time = cfg_getint(cfg_sec, "time");
				char* tmp = cfg_getstr(cfg_sec, "model");
				if (tmp) w->model = mod->getAssimpModel(tmp);
				w->range = cfg_getfloat(cfg_sec, "range");
				w->damage = cfg_getfloat(cfg_sec, "unit_damage");		// TODO: merge unit/wall damage
			}
			break;

		case WEAPON_TYPE_PROXI_MINE:
			{
				WeaponProxiMine* w = new WeaponProxiMine();
				cfg_sec = cfg_getnsec(cfg_item, "mine", 0);
				wt = w;

				char* tmp = cfg_getstr(cfg_sec, "model");
				if (tmp) w->model = mod->getAssimpModel(tmp);
				w->range = cfg_getfloat(cfg_sec, "range");
				w->damage = cfg_getfloat(cfg_sec, "unit_damage");		// TODO: merge unit/wall damage
			}
			break;

		case WEAPON_TYPE_REMOTE_MINE:
			{
				WeaponRemoteMine* w = new WeaponRemoteMine();
				cfg_sec = cfg_getnsec(cfg_item, "mine", 0);
				wt = w;

				char* tmp = cfg_getstr(cfg_sec, "model");
				if (tmp) w->model = mod->getAssimpModel(tmp);
				w->range = cfg_getfloat(cfg_sec, "range");
				w->damage = cfg_getfloat(cfg_sec, "unit_damage");		// TODO: merge unit/wall damage
			}
			break;

		case WEAPON_TYPE_REMOTE_TRIG:
			wt = new WeaponRemoteTrigger();
			break;

		case WEAPON_TYPE_ROCKET:
			{
				WeaponRocket* w = new WeaponRocket();
				cfg_sec = cfg_getnsec(cfg_item, "rocket", 0);
				wt = w;

				char* tmp = cfg_getstr(cfg_sec, "model");
				if (tmp) w->model = mod->getAssimpModel(tmp);
				// TODO: blast radius vs detection radius?
				w->range = cfg_getfloat(cfg_sec, "range");
				w->damage = cfg_getfloat(cfg_sec, "unit_damage");		// TODO: merge unit/wall damage
			}
			break;

		case WEAPON_TYPE_ATTRACTOR:
			{
				WeaponAttractor* w = new WeaponAttractor();
				cfg_sec = cfg_getnsec(cfg_item, "attractor", 0);
				wt = w;

				char* tmp = cfg_getstr(cfg_sec, "model");
				if (tmp) w->model = mod->getAssimpModel(tmp);
				w->range = cfg_getfloat(cfg_sec, "range");
				w->time = cfg_getint(cfg_sec, "time");
				w->force = Quadratic(3.0f, 3.0f, 3.0f);		// TODO configuration
			}
			break;

		default:
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
	this->model = NULL;
}

