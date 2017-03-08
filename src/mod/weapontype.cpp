// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <confuse.h>
#include "../rage.h"
#include "../util/quadratic.h"
#include "mod.h"
#include "weapontype.h"
#include "confuse_types.h"
#include "../weapons/weapons.h"
#include "../game_engine.h"
#include "../render/render.h"

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
	CFG_STR((char*) "crosshair", 0, CFGF_NONE),
	CFG_INT((char*) "crosshair_min", 10, CFGF_NONE),
	CFG_INT((char*) "crosshair_max", 20, CFGF_NONE),
	CFG_FLOAT_LIST((char*) "zoom_levels", (char*)"", CFGF_NONE),
	CFG_STR((char*) "model", 0, CFGF_NONE),

	// Sounds
	CFG_SEC((char*) "sound", weaponsound_opts, CFGF_MULTI),

	// Attachment locations (3D coord on weapon to place attachment)
	CFG_FLOAT_LIST((char*) "loc-top", (char*)"{0.0, 0.0, 0.0}", CFGF_NONE),
	CFG_FLOAT_LIST((char*) "loc-left", (char*)"{0.0, 0.0, 0.0}", CFGF_NONE),
	CFG_FLOAT_LIST((char*) "loc-right", (char*)"{0.0, 0.0, 0.0}", CFGF_NONE),
	CFG_FLOAT_LIST((char*) "loc-barrel", (char*)"{0.0, 0.0, 0.0}", CFGF_NONE),

	// The per-type settings fall into one of five categories:
	CFG_SEC((char*) "raycast", weapconf_raycast, CFGF_NONE),
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
	cfg_t* cfg_sec;
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
				w->range = (float)cfg_getfloat(cfg_sec, "range");
				w->damage = (float)cfg_getfloat(cfg_sec, "damage");
				w->burst = (float)cfg_getint(cfg_sec, "burst");
				w->speed = (float)cfg_getfloat(cfg_sec, "speed");

				// Damage in config file is across the whole shot
				// rather than per raycast in a burst
				w->damage = w->damage / w->burst;
			}
			break;

		case WEAPON_TYPE_FLAMETHROWER:
			{
				WeaponFlamethrower* w = new WeaponFlamethrower();
				cfg_sec = cfg_getnsec(cfg_item, "raycast", 0);
				wt = w;

				w->angle_range = cfg_getint(cfg_sec, "angle_range");
				w->range = (float)cfg_getfloat(cfg_sec, "range");
				w->damage = (float)cfg_getfloat(cfg_sec, "damage");
				w->burst = (float)cfg_getint(cfg_sec, "burst");

				// Again, damage is across all rays
				w->damage = w->damage / w->burst;
			}
			break;

		case WEAPON_TYPE_TIMED_MINE:
			{
				WeaponTimedMine* w = new WeaponTimedMine();
				cfg_sec = cfg_getnsec(cfg_item, "mine", 0);
				wt = w;

				w->time = cfg_getint(cfg_sec, "time");
				char* tmp = cfg_getstr(cfg_sec, "ammo-model");
				if (tmp) w->ammo_model = mod->getAssimpModel(tmp);
				w->range = (float)cfg_getfloat(cfg_sec, "range");
				w->damage = (float)cfg_getfloat(cfg_sec, "damage");
			}
			break;

		case WEAPON_TYPE_PROXI_MINE:
			{
				WeaponProxiMine* w = new WeaponProxiMine();
				cfg_sec = cfg_getnsec(cfg_item, "mine", 0);
				wt = w;

				char* tmp = cfg_getstr(cfg_sec, "ammo-model");
				if (tmp) w->ammo_model = mod->getAssimpModel(tmp);
				w->range = (float)cfg_getfloat(cfg_sec, "range");
				w->damage = (float)cfg_getfloat(cfg_sec, "damage");
			}
			break;

		case WEAPON_TYPE_REMOTE_MINE:
			{
				WeaponRemoteMine* w = new WeaponRemoteMine();
				cfg_sec = cfg_getnsec(cfg_item, "mine", 0);
				wt = w;

				char* tmp = cfg_getstr(cfg_sec, "ammo-model");
				if (tmp) w->ammo_model = mod->getAssimpModel(tmp);
				w->range = (float)cfg_getfloat(cfg_sec, "range");
				w->damage = (float)cfg_getfloat(cfg_sec, "damage");
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

				char* tmp = cfg_getstr(cfg_sec, "ammo-model");
				if (tmp) w->ammo_model = mod->getAssimpModel(tmp);
				w->range = (float)cfg_getfloat(cfg_sec, "range");		// TODO: blast radius vs detection radius?
				w->damage = (float)cfg_getfloat(cfg_sec, "damage");
			}
			break;

		case WEAPON_TYPE_ATTRACTOR:
			{
				WeaponAttractor* w = new WeaponAttractor();
				cfg_sec = cfg_getnsec(cfg_item, "attractor", 0);
				wt = w;

				char* tmp = cfg_getstr(cfg_sec, "ammo-model");
				if (tmp) w->ammo_model = mod->getAssimpModel(tmp);
				w->range = (float)cfg_getfloat(cfg_sec, "range");
				w->time = cfg_getint(cfg_sec, "time");
				w->force = Quadratic(3.0f, 3.0f, 3.0f);		// TODO configuration
				w->inwards = (cfg_getint(cfg_sec, "inwards") == 1);
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

	// Common weapon settings
	wt->fire_delay = cfg_getint(cfg_item, "fire_delay");
	wt->reload_delay = cfg_getint(cfg_item, "reload_delay");
	wt->continuous = (cfg_getint(cfg_item, "continuous") == 1);
	wt->magazine_limit = cfg_getint(cfg_item, "magazine_limit");
	wt->belt_limit = cfg_getint(cfg_item, "belt_limit");

	// Crosshair image and sizes
	char* tmp = cfg_getstr(cfg_item, "crosshair");
	if (tmp) {
		string name = "crosshairs/";
		name.append(tmp);
		wt->crosshair = GEng()->render->loadSprite(name, mod);
		wt->crosshair_min = cfg_getint(cfg_item, "crosshair_min");
		wt->crosshair_max = cfg_getint(cfg_item, "crosshair_max");
	}

	// 3D model for pickups (and maybe also in first-person view one day)
	tmp = cfg_getstr(cfg_item, "model");
	if (tmp) {
		wt->model = mod->getAssimpModel(tmp, ALF_NoRecenter);
	}

	wt->attach_loc[WPATT_TOP] = cfg_getvec3(cfg_item, "loc-top");
	wt->attach_loc[WPATT_LEFT] = cfg_getvec3(cfg_item, "loc-left");
	wt->attach_loc[WPATT_RIGHT] = cfg_getvec3(cfg_item, "loc-right");
	wt->attach_loc[WPATT_BARREL] = cfg_getvec3(cfg_item, "loc-barrel");

	// Load sounds
	int num_sounds = cfg_size(cfg_item, "sound");
	for (j = 0; j < num_sounds; j++) {
		cfg_t* cfg_sound = cfg_getnsec(cfg_item, "sound", j);

		WeaponTypeSound* wts = new WeaponTypeSound();
		wts->type = cfg_getint(cfg_sound, "type");

		char * tmp = cfg_getstr(cfg_sound, "sound");
		if (tmp == NULL) {
			mod->setLoadErr("No value for field 'sound'");
			delete(wts);
			delete(wt);
			return NULL;
		}

		wts->snd = mod->getSound(tmp);
		if (wts->snd == NULL) {
			mod->setLoadErr("Unable to load sound file");
			delete(wts);
			delete(wt);
			return NULL;
		}

		wt->sounds.push_back(wts);
	}

	// Load zoom levels. Level 0 is always no zoom
	wt->zoom_levels.push_back(0.0f);
	int num_zooms = cfg_size(cfg_item, "zoom_levels");
	for (j = 0; j < num_zooms; j++) {
		wt->zoom_levels.push_back((float)cfg_getnfloat(cfg_item, "zoom_levels", j));
	}

	return wt;
}


/**
* Returns a random sound which matches the specified type.
* If it can't find a sound for that type, return NULL
**/
AudioPtr WeaponType::getSound(int type)
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
	this->st = NULL;
	this->type = -1;
	this->fire_delay = 0;
	this->reload_delay = 0;
	this->continuous = false;
	this->magazine_limit = 0;
	this->belt_limit = 0;
	this->model = NULL;
	this->ammo_model = NULL;
	this->crosshair = NULL;
	this->crosshair_min = 0;
	this->crosshair_max = 0;
}


/**
* Can an attachment be made at this location
**/
bool WeaponType::canAttach(WeaponAttachmentLocation loc)
{
	return true;
}
