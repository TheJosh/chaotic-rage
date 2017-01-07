// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <confuse.h>
#include "weaponattachment.h"
#include "mod.h"
#include "confuse_types.h"
using namespace std;


/**
* Config file opts
**/
cfg_opt_t weaponattachment_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_STR((char*) "model", 0, CFGF_NONE),

	// Attachable locations (3D coord on model to attach to weapon)
	CFG_FLOAT_LIST((char*) "loc-top", (char*)"{0.0, 0.0, 0.0}", CFGF_NONE),
	CFG_FLOAT_LIST((char*) "loc-left", (char*)"{0.0, 0.0, 0.0}", CFGF_NONE),
	CFG_FLOAT_LIST((char*) "loc-right", (char*)"{0.0, 0.0, 0.0}", CFGF_NONE),
	CFG_FLOAT_LIST((char*) "loc-barrel", (char*)"{0.0, 0.0, 0.0}", CFGF_NONE),

	CFG_END()
};


/**
* Item loading function handler
**/
WeaponAttachment* loadItemWeaponAttachment(cfg_t* cfg_item, Mod* mod)
{
	WeaponAttachment* wa = new WeaponAttachment();
	wa->name = cfg_getstr(cfg_item, "name");

	char* tmp = cfg_getstr(cfg_item, "model");
	if (tmp == NULL) {
		delete wa;
		return NULL;
	}
	wa->model = mod->getAssimpModel(tmp);

	wa->loc[WPATT_TOP] = cfg_getvec3(cfg_item, "loc-top");
	wa->loc[WPATT_LEFT] = cfg_getvec3(cfg_item, "loc-left");
	wa->loc[WPATT_RIGHT] = cfg_getvec3(cfg_item, "loc-right");
	wa->loc[WPATT_BARREL] = cfg_getvec3(cfg_item, "loc-barrel");

	return wa;
}


/**
* Can this attachment be used in the given location?
**/
bool WeaponAttachment::canAttach(WeaponAttachmentLocation loc)
{
	return true;
}
