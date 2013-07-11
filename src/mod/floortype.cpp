// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include "../rage.h"
#include "../gamestate.h"
#include "mod.h"
#include "floortype.h"

using namespace std;


/**
* Config file opts
**/
cfg_opt_t floortype_opts[] =
{
	CFG_STR((char*) "name", (char*)"", CFGF_NONE),
	CFG_STR((char*) "image", (char*)"", CFGF_NONE),
	CFG_INT((char*) "stretch", 0, CFGF_NONE),		// 0 = tile, 1 = stretch
	CFG_END()
};


/**
* Item loading function handler
**/
FloorType* loadItemFloorType(cfg_t *cfg_areatype, Mod * mod)
{
	FloorType* at;
	string filename;
	
	filename = "floortypes/";
	filename.append(cfg_getstr(cfg_areatype, "image"));
	filename.append("-fr0.png");
	
	at = new FloorType();
	at->name = cfg_getstr(cfg_areatype, "name");
	at->stretch = (cfg_getint(cfg_areatype, "stretch") == 1);
	at->texture = mod->st->render->loadSprite(filename.c_str(), mod);
	
	return at;
}


/**
* Constructor
**/
FloorType::FloorType()
{
	this->texture = NULL;
}
