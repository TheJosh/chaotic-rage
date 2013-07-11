// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include "../rage.h"
#include "aitype.h"
#include "mod.h"


using namespace std;


/**
* Config file opts
**/
cfg_opt_t ai_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_STR((char*) "script", 0, CFGF_NONE),
	CFG_END()
};


/**
* Item loading function handler
**/
AIType* loadItemAIType(cfg_t* cfg_item, Mod* mod)
{
	AIType* ai;
	string filename;
	
	ai = new AIType();
	ai->name = cfg_getstr(cfg_item, "name");
	
	filename = "ais/";
	filename.append(cfg_getstr(cfg_item, "script"));
	filename.append(".lua");
	
	ai->script = mod->loadText(filename);
	
	return ai;
}


AIType::AIType()
{
}
