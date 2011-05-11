// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include "rage.h"

using namespace std;


/**
* Config file opts
**/
cfg_opt_t gametype_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_STR((char*) "script", 0, CFGF_NONE),
	CFG_END()
};


/**
* Item loading function handler
**/
GameType* loadItemGameType(cfg_t* cfg_item, Mod* mod)
{
	GameType* gt;
	string filename;
	
	gt = new GameType();
	gt->name = cfg_getstr(cfg_item, "name");
	
	filename = "gametypes/";
	filename.append(cfg_getstr(cfg_item, "script"));
	filename.append(".lua");
	
	gt->script = mod->loadText(filename);
	
	return gt;
}


GameType::GameType()
{
}
