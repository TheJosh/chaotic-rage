// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <confuse.h>
#include "../rage.h"
#include "aitype.h"
#include "mod.h"


using namespace std;


/**
* Config file opts
**/
cfg_opt_t ai_opts[] =
{
	CFG_STR((char*) "name", (char*)"", CFGF_NONE),
	CFG_STR((char*) "script", (char*)"", CFGF_NONE),
	CFG_END()
};


/**
* Item loading function handler
**/
AIType* loadItemAIType(cfg_t* cfg_item, Mod* mod)
{
	AIType* ai;
	string filename;
	char *tmp;

	ai = new AIType();

	// Name field
	tmp = cfg_getstr(cfg_item, "name");
	if (tmp == NULL) {
		mod->setLoadErr("Missing field 'name'");
		return NULL;
	}
	ai->name = std::string(tmp);

	// Script field
	tmp = cfg_getstr(cfg_item, "script");
	if (tmp == NULL) {
		mod->setLoadErr("Missing field 'script'");
		return NULL;
	}

	// Script filename
	filename = "ais/";
	filename.append(std::string(tmp));
	filename.append(".lua");

	// Try and load the script from file
	tmp = mod->loadText(filename);
	if (tmp == NULL) {
		mod->setLoadErr("Unable to load stript: %s for ai %s", filename.c_str(), ai->name.c_str());
		return NULL;
	}
	ai->script = std::string(tmp);
	free(tmp);

	return ai;
}


AIType::AIType()
{
}
