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
cfg_opt_t weapontype_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_INT((char*) "particlegen", -1, CFGF_NONE),
	CFG_INT((char*) "melee", 0, CFGF_NONE),
	CFG_INT((char*) "damage", 0, CFGF_NONE),
	CFG_STR((char*) "sound_fire", (char*)"", CFGF_NONE),
	CFG_END()
};


/**
* Item loading function handler
**/
WeaponType* loadItemWeaponType(cfg_t* cfg_item, Mod* mod)
{
	WeaponType* wt;
	string filename;
	
	wt = new WeaponType();
	wt->name = cfg_getstr(cfg_item, "name");
	wt->melee = cfg_getint(cfg_item, "melee");
	
	if (wt->melee == 0) {
		// Bullet
		if (cfg_getint(cfg_item, "particlegen") != -1) {
			wt->pg = mod->getParticleGenType(cfg_getint(cfg_item, "particlegen"));
		}
		
	} else if (wt->melee == 1) {
		// Melee
		wt->damage = cfg_getint(cfg_item, "damage");
	}
	
	// Load large icon
	filename = getDataDirectory(DF_WEAPONS);
	filename.append(cfg_getstr(cfg_item, "name"));
	filename.append(".png");
	wt->icon_large = mod->st->render->loadSprite(filename, mod);
	
	// Load firing sound
	char * tmp = cfg_getstr(cfg_item, "sound_fire");
	if (tmp == NULL) return NULL;
	wt->sound_fire = mod->getSound(tmp);
	
	return wt;
}



WeaponType::WeaponType()
{
	this->pg = NULL;
	this->icon_large = NULL;
}
