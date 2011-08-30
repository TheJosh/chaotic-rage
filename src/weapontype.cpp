// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include "rage.h"

using namespace std;


// Sound section
static cfg_opt_t weaponsound_opts[] =
{
	CFG_STR((char*) "sound", 0, CFGF_NONE),
	CFG_INT((char*) "type", 0, CFGF_NONE),
	CFG_END()
};

/**
* Config file opts
**/
cfg_opt_t weapontype_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_INT((char*) "particlegen", -1, CFGF_NONE),
	CFG_INT((char*) "melee", 0, CFGF_NONE),
	CFG_INT((char*) "damage", 0, CFGF_NONE),
	
	CFG_SEC((char*) "sound", weaponsound_opts, CFGF_MULTI),
	
	CFG_END()
};


/**
* Item loading function handler
**/
WeaponType* loadItemWeaponType(cfg_t* cfg_item, Mod* mod)
{
	WeaponType* wt;
	cfg_t *cfg_sound;
	string filename;
	int j;
	
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
	
	
	/// Sounds ///
	int num_sounds = cfg_size(cfg_item, "sound");
	
	// load sounds
	for (j = 0; j < num_sounds; j++) {
		cfg_sound = cfg_getnsec(cfg_item, "sound", j);
		
		WeaponTypeSound* wts = new WeaponTypeSound();
		
		wts->type = cfg_getint(cfg_sound, "type");
		
		char * tmp = cfg_getstr(cfg_sound, "sound");
		if (tmp == NULL) return NULL;
		wts->snd = mod->getSound(tmp);
		
		wt->sounds.push_back(wts);
	}
	
	return wt;
}


/**
* Returns a random sound which matches the specified type.
* If it can't find a sound for that type, return NULL
**/
Sound* WeaponType::getSound(int type)
{
	unsigned int j = 0;
	unsigned int num = 0;
	
	// Find out how many of this time exist
	for (j = 0; j < this->sounds.size(); j++) {
		if (this->sounds.at(j)->type == type) num++;
	}
	
	// Randomly choose one
	num = getRandom(0, num);
	for (j = 0; j < this->sounds.size(); j++) {
		if (this->sounds.at(j)->type == type) {
			if (num == 0) {
				return this->sounds.at(j)->snd;
			}
			num--;
		}
	}
	
	return NULL;
}


WeaponType::WeaponType()
{
	this->pg = NULL;
	this->icon_large = NULL;
}
