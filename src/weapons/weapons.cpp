// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <confuse.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include "../rage.h"
#include "../gamestate.h"
#include "../util/quadratic.h"
#include "../map.h"
#include "../fx/newparticle.h"
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
	CFG_END()
};


/**
* WEAPON_TYPE_DIGDOWN
**/
cfg_opt_t weapconf_digdown[] =
{
	CFG_INT((char*) "radius", 2, CFGF_NONE),
	CFG_FLOAT((char*) "depth", -0.1, CFGF_NONE),
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
	CFG_STR((char*) "model", 0, CFGF_NONE),
	CFG_FLOAT((char*) "damage", 10, CFGF_NONE),
	CFG_FLOAT((char*) "range", 50, CFGF_NONE),
	CFG_END()
};


/**
* WEAPON_TYPE_ROCKET
**/
cfg_opt_t weapconf_rocket[] =
{
	CFG_STR((char*) "model", 0, CFGF_NONE),
	CFG_FLOAT((char*) "range", 50, CFGF_NONE),
	CFG_FLOAT((char*) "damage", 10, CFGF_NONE),
	CFG_END()
};


/**
* WEAPON_TYPE_ATTRACTOR
**/
cfg_opt_t weapconf_attractor[] =
{
	CFG_STR((char*) "model", 0, CFGF_NONE),
	CFG_FLOAT((char*) "range", 50, CFGF_NONE),
	CFG_INT((char*) "time", 1000, CFGF_NONE),
	CFG_END()
};


