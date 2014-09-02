// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <list>
#include <confuse.h>
#include <btBulletDynamicsCommon.h>
#include "pickuptype.h"
#include "../rage.h"
#include "../render_opengl/assimpmodel.h"
#include "../game_state.h"
#include "../entity/unit.h"
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
	CFG_BOOL((char*) "invincible", cfg_false, CFGF_NONE),
	CFG_FLOAT((char*) "weapon-damage", 1.0f, CFGF_NONE),
	CFG_END()
};


/**
* Combos, for extra funness
**/
cfg_opt_t combo_opts[] =
{
	CFG_STR((char*) "second", (char*)"", CFGF_NONE),
	CFG_STR((char*) "benefit", (char*)"", CFGF_NONE),
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
	CFG_SEC((char*) "combo", combo_opts, CFGF_MULTI),

	CFG_END()
};


/**
* Set up
**/
PickupType::PickupType()
{
	this->model = NULL;
	this->col_shape = NULL;
	this->type = -1;
	this->wt = NULL;
	this->perm = NULL;
	this->temp = NULL;
	this->delay = 0;
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
* Sets the model, creates a box shape too
**/
void PickupType::setModel(AssimpModel *model)
{
	this->model = model;
	this->col_shape = this->model->getCollisionShape();
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
		if (! pt->model) {
			delete(pt);
			return NULL;
		}
		pt->col_shape = pt->model->getCollisionShape();
	}

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
		
		// Combos will be hooked up later
		if (cfg_size(cfg_item, "combo") > 0) {
			PowerupCombo combo;
			for (unsigned int i = 0; i < cfg_size(cfg_item, "combo"); ++i) {
				cfg_t* cfg_combo = cfg_getnsec(cfg_item, "combo", i);
				combo.second_name = std::string(cfg_getstr(cfg_combo, "second"));
				combo.benefit_name = std::string(cfg_getstr(cfg_combo, "benefit"));
				pt->combos.push_back(combo);
			}
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

	pt->health = (float)cfg_getfloat(cfg, "health");
	pt->max_speed = (float)cfg_getfloat(cfg, "max-speed");
	pt->melee_damage = (float)cfg_getfloat(cfg, "melee-damage");
	pt->melee_delay = (float)cfg_getfloat(cfg, "melee-delay");
	pt->melee_cooldown = (float)cfg_getfloat(cfg, "melee-cooldown");
	pt->invincible = (cfg_getbool(cfg, "invincible") == cfg_true);
	pt->weapon_damage = (float)cfg_getfloat(cfg, "weapon-damage");

	return pt;
}


/**
* Handle a pickup by a unit.
*
* @return True on success, false on failure (e.g. wrong weapon for ammo box)
**/
bool PickupType::doUse(Unit *u)
{
	GameState *st = u->getGameState();

	switch (this->type) {
		case PICKUP_TYPE_WEAPON:
			assert(this->wt);
			st->addHUDMessage(u->slot, "Picked up a ", this->wt->title);
			u->pickupWeapon(this->wt);
			break;

		case PICKUP_TYPE_AMMO:
			{
				bool success = false;
				if (this->wt == NULL) {
					WeaponType *wt = u->getWeaponTypeCurr();
					if (wt == NULL) return false;
					success = u->pickupAmmo(wt);
				} else {
					success = u->pickupAmmo(this->wt);
				}

				if (! success) return false;

				st->addHUDMessage(u->slot, "Picked up some ammo");
			}
			break;

		case PICKUP_TYPE_POWERUP:
			u->applyPickupAdjust(this->perm);
			u->applyPickupAdjust(this->temp);
			u->addActivePickup(this);
			if (!this->title.empty()) {
				st->addHUDMessage(u->slot, "Picked up a ", this->title);
			}

			// Apply any combos
			for (vector<PowerupCombo>::iterator it = this->combos.begin(); it != this->combos.end(); ++it) {
				if ((*it).benefit != NULL && u->hasActivePickup((*it).second)) {
					(*it).benefit->doUse(u);
				}
			}
			break;

		case PICKUP_TYPE_CURSOR:
			return false;
			break;

		default:
			assert(0);
	}

	return true;
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
