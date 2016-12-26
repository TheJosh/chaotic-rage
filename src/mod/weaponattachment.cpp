// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <confuse.h>
#include "weaponattachment.h"
#include "mod.h"

using namespace std;


/**
* Config file opts
**/
cfg_opt_t weaponattachment_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_END()
};


/**
* Item loading function handler
**/
WeaponAttachment* loadItemWeaponAttachment(cfg_t* cfg_item, Mod* mod)
{
	WeaponAttachment* wa = new WeaponAttachment();
	wa->name = cfg_getstr(cfg_item, "name");
	
	return wa;
}
