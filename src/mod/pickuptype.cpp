// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <list>
#include <confuse.h>
#include <zzip/zzip.h>
#include <btBulletDynamicsCommon.h>
#include "pickuptype.h"
#include "../rage.h"
#include "../render/assimpmodel.h"
#include "../gamestate.h"
#include "mod.h"
#include "weapontype.h"


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
	CFG_FLOAT((char*) "max-speed", 1.0f, CFGF_NONE),
	CFG_FLOAT((char*) "melee-damage", 1.0f, CFGF_NONE),
	CFG_FLOAT((char*) "melee-delay", 1.0f, CFGF_NONE),
	CFG_FLOAT((char*) "melee-cooldown", 1.0f, CFGF_NONE),
	CFG_END()
};


/**
* Config file opts
**/
cfg_opt_t pickuptype_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_STR((char*) "title", (char*)"Powerup", CFGF_NONE),
	CFG_STR((char*) "model", 0, CFGF_NONE),
	CFG_INT((char*) "type", 0, CFGF_NONE),

	CFG_SEC((char*) "perm", adjust_opts, CFGF_NONE),
	CFG_SEC((char*) "temp", adjust_opts, CFGF_NONE),
	CFG_INT((char*) "delay", 15 * 1000, CFGF_NONE),		// default 15 secs

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
	this->col_shape = NULL;
}

/**
* Tear down
**/
PickupType::~PickupType()
{
	delete(this->perm);
	delete(this->temp);
	delete(this->col_shape);
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
	
	// Load the collision shape
	btVector3 sizeHE = pt->model->getBoundingSizeHE();
	pt->col_shape = new btBoxShape(sizeHE);
	
	// Powerups have a bunch more fields
	if (pt->type == PICKUP_TYPE_POWERUP) {
		pt->title = std::string(cfg_getstr(cfg_item, "title"));
		pt->delay = cfg_getint(cfg_item, "delay");

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
* Load a PickupTypeAdjust
**/
PickupTypeAdjust* PickupType::loadAdjust(cfg_t* cfg)
{
	PickupTypeAdjust* pt = new PickupTypeAdjust();

	pt->health = cfg_getfloat(cfg, "health");
	pt->max_speed = cfg_getfloat(cfg, "max-speed");
	pt->melee_damage = cfg_getfloat(cfg, "melee-damage");
	pt->melee_delay = cfg_getfloat(cfg, "melee-delay");
	pt->melee_cooldown = cfg_getfloat(cfg, "melee-cooldown");
	
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
			u->applyPickupAdjust(this->perm);
			u->applyPickupAdjust(this->temp);
			st->addHUDMessage(u->slot, "Picked up a ", this->title);
			break;

		default:
			assert(0);
	}
}


/**
* Apply an adjustment to a unit
**/
void PickupType::finished(Unit *u)
{
	if (this->type == PICKUP_TYPE_POWERUP) {
		u->rollbackPickupAdjust(this->temp);
	}
}

