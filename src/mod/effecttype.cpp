// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <list>
#include <confuse.h>
#include "../rage.h"
#include "effecttype.h"
#include "mod.h"

using namespace std;


/**
* Config file opts
**/
cfg_opt_t effecttype_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_END()
};


/**
* Set up
**/
EffectType::EffectType()
{
}


/**
* Tear down
**/
EffectType::~EffectType()
{
}


/**
* Item loading function handler
**/
EffectType* loadItemEffectType(cfg_t* cfg_item, Mod* mod)
{
	EffectType* c = new EffectType();
	c->name = cfg_getstr(cfg_item, "name");

	return c;
}

