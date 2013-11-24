// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <confuse.h>
#include "../rage.h"
#include "mod.h"
#include "gametype.h"

using namespace std;


/**
* Config file opts
**/
cfg_opt_t gametype_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_STR((char*) "title", 0, CFGF_NONE),
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
	gt->title = cfg_getstr(cfg_item, "title");
	
	filename = "gametypes/";
	filename.append(cfg_getstr(cfg_item, "script"));
	filename.append(".lua");
	
	char * tmp = mod->loadText(filename);
	gt->script = std::string(tmp);
	free(tmp);

	return gt;
}


GameType::GameType()
{
}
