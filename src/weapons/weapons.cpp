// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <confuse.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include "../rage.h"
#include "../game_state.h"
#include "../util/quadratic.h"
#include "../fx/effects_manager.h"
#include "weapons.h"

using namespace std;



/**
* WEAPON_TYPE_RAYCAST
* WEAPON_TYPE_FLAMETHROWER
**/
cfg_opt_t weapconf_raycast[] =
{
	CFG_INT((char*) "angle_range", 0, CFGF_NONE),
	CFG_FLOAT((char*) "range", 50, CFGF_NONE),
	CFG_FLOAT((char*) "damage", 10, CFGF_NONE),
	CFG_INT((char*) "burst", 1, CFGF_NONE),
	CFG_FLOAT((char*) "speed", 350.0f, CFGF_NONE),
	CFG_END()
};


/**
* WEAPON_TYPE_TIMED_MINE
* WEAPON_TYPE_PROXI_MINE
* WEAPON_TYPE_REMOTE_MINE
**/
cfg_opt_t weapconf_mine[] =
{
	CFG_INT((char*) "time", 1000, CFGF_NONE),
	CFG_STR((char*) "ammo-model", 0, CFGF_NONE),
	CFG_FLOAT((char*) "damage", 10, CFGF_NONE),
	CFG_FLOAT((char*) "range", 50, CFGF_NONE),
	CFG_END()
};


/**
* WEAPON_TYPE_ROCKET
**/
cfg_opt_t weapconf_rocket[] =
{
	CFG_STR((char*) "ammo-model", 0, CFGF_NONE),
	CFG_FLOAT((char*) "range", 50, CFGF_NONE),
	CFG_FLOAT((char*) "damage", 10, CFGF_NONE),
	CFG_END()
};


/**
* WEAPON_TYPE_ATTRACTOR
**/
cfg_opt_t weapconf_attractor[] =
{
	CFG_STR((char*) "ammo-model", 0, CFGF_NONE),
	CFG_FLOAT((char*) "range", 50, CFGF_NONE),
	CFG_INT((char*) "time", 1000, CFGF_NONE),
	CFG_INT((char*) "inwards", 1, CFGF_NONE),		// 1 = attract, 2 = repel
	CFG_END()
};
