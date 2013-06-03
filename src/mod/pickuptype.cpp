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
* Config file opts
**/
cfg_opt_t pickuptype_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_STR((char*) "model", 0, CFGF_NONE),
	CFG_INT((char*) "type", 0, CFGF_NONE),
	CFG_END()
};


/**
* Set up
**/
PickupType::PickupType()
{
	this->wt = NULL;
}

/**
* Tear down
**/
PickupType::~PickupType()
{
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
	
	char * tmp = cfg_getstr(cfg_item, "model");
	if (tmp != NULL) {
		pt->model = mod->getAssimpModel(tmp);
		if (! pt->model) return NULL;
	}
	
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

		default:
			assert(0);
	}
}