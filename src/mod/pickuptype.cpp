// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <list>
#include <confuse.h>
#include <zzip/zzip.h>
#include "pickuptype.h"
#include "../rage.h"

using namespace std;


/**
* Unit property adjustments
*
* The current values get MULTIPLIED by these values
* so use 1.0 for 100%, 1.5 for 150%, 0.5 for 50%, etc.
**/
cfg_opt_t adjust_opts[] =
{
	CFG_FLOAT((char*) "health", 1.0f, CFGF_NONE),
	CFG_END()
};


/**
* Config file opts
**/
cfg_opt_t pickuptype_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_STR((char*) "title", 0, CFGF_NONE),
	CFG_STR((char*) "model", 0, CFGF_NONE),
	CFG_INT((char*) "type", 0, CFGF_NONE),

	CFG_SEC((char*) "perm", adjust_opts, CFGF_NONE),
	CFG_SEC((char*) "temp", adjust_opts, CFGF_NONE),

	CFG_END()
};


/**
* Set up
**/
PickupType::PickupType()
{
	this->wt = NULL;
	this->perm = NULL;
	this->temp = NULL;
}

/**
* Tear down
**/
PickupType::~PickupType()
{
	delete(this->perm);
	delete(this->temp);
}


/**
* Item loading function handler
**/
PickupType* loadItemPickupType(cfg_t* cfg_item, Mod* mod)
{
	PickupType* pt;

	pt = new PickupType();
	pt->name = cfg_getstr(cfg_item, "name");
	pt->type = cfg_getint(cfg_item, "type");
	
	// Load model
	char * tmp = cfg_getstr(cfg_item, "model");
	if (tmp != NULL) {
		pt->model = mod->getAssimpModel(tmp);
		if (! pt->model) return NULL;
	}
	
	// Load perm and temp adjustments
	if (pt->type == PICKUP_TYPE_POWERUP) {
		tmp = cfg_getstr(cfg_item, "title");
		if (tmp != NULL) {
			pt->title = std::string(tmp);
		} else {
			pt->title = "Powerup";
		}

		if (cfg_size(cfg_item, "perm") > 0) {
			pt->perm = pt->loadAdjust(cfg_getnsec(cfg_item, "perm", 0));
		}

		if (cfg_size(cfg_item, "temp") > 0) {
			pt->temp = pt->loadAdjust(cfg_getnsec(cfg_item, "temp", 0));
		}
	}

	return pt;
}


/**
* Load a PickupTypeAdjust from libConfuse
**/
PickupTypeAdjust* PickupType::loadAdjust(cfg_t* cfg)
{
	PickupTypeAdjust* pt = new PickupTypeAdjust();

	pt->health = cfg_getfloat(cfg, "health");

	return pt;
}


/**
* Handle a pickup by a unit
**/
void PickupType::doUse(Unit *u)
{
	GameState *st = u->getGameState();

	switch (this->type) {
		case PICKUP_TYPE_WEAPON:
			assert(this->wt);
			st->addHUDMessage(u->slot, "Picked up a ", this->wt->title);
			u->pickupWeapon(this->wt);
			break;

		case PICKUP_TYPE_AMMO:
			if (this->wt == NULL) {
				u->pickupAmmo(u->getWeaponTypeCurr());
			} else {
				u->pickupAmmo(this->wt);
			}

			st->addHUDMessage(u->slot, "Picked up some ammo");
			break;

		case PICKUP_TYPE_POWERUP:

			// TODO: apply perm and temp adjustments

			st->addHUDMessage(u->slot, "Picked up a ", this->title);
			break;

		default:
			assert(0);
	}
}