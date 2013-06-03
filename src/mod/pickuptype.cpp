// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <list>
#include <confuse.h>
#include <zzip/zzip.h>
#include "campaign.h"
#include "../rage.h"

using namespace std;


/**
* Config file opts
**/
cfg_opt_t pickuptype_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_END()
};


/**
* Set up
**/
PickupType::PickupType()
{
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
	
	char * tmp = cfg_getstr(cfg_item, "model");
	if (tmp != NULL) {
		pt->model = mod->getAssimpModel(tmp);
		if (! pt->model) return NULL;
	}
	
	return pt;
}


