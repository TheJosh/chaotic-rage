// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <confuse.h>
#include <string>
#include <btBulletDynamicsCommon.h>
#include "../rage.h"
#include "mod.h"
#include "unittype.h"

using namespace std;



// Animation section
static cfg_opt_t unitanim_opts[] =
{
	CFG_INT((char*) "type", 0, CFGF_NONE),		// UNIT_ANIM_*
	CFG_INT((char*) "animation", 0, CFGF_NONE),
	CFG_INT((char*) "start_frame", 0, CFGF_NONE),
	CFG_INT((char*) "end_frame", 0, CFGF_NONE),
	CFG_INT((char*) "loop", 1, CFGF_NONE),
	CFG_END()
};

// Sound section
static cfg_opt_t unitsound_opts[] =
{
	CFG_INT((char*) "type", 0, CFGF_NONE),		// UNIT_SOUND_*
	CFG_STR((char*) "sound", 0, CFGF_NONE),
	CFG_END()
};


// Unitclass section
cfg_opt_t unittype_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_STR((char*) "model", 0, CFGF_NONE),
	
	CFG_FLOAT((char*) "max_speed", 0.0f, CFGF_NONE),
	CFG_INT((char*) "melee_damage", 1000, CFGF_NONE),
	CFG_INT((char*) "melee_delay", 500, CFGF_NONE),
	CFG_INT((char*) "melee_cooldown", 1000, CFGF_NONE),
	CFG_INT((char*) "special_delay", 500, CFGF_NONE),
	CFG_INT((char*) "special_cooldown", 1000, CFGF_NONE),
	
	CFG_SEC((char*) "animation", unitanim_opts, CFGF_MULTI),
	CFG_SEC((char*) "sound", unitsound_opts, CFGF_MULTI),
	CFG_STR_LIST((char*) "spawn_weapons", 0, CFGF_NONE),
	CFG_STR((char*) "special_weapon", 0, CFGF_NONE),

	CFG_INT((char*) "playable", 1, CFGF_NONE),
	CFG_FLOAT((char*) "health", 0, CFGF_NONE),

	CFG_END()
};


/**
* Item loading function handler
**/
UnitType* loadItemUnitType(cfg_t* cfg_item, Mod* mod)
{
	UnitType* uc;
	int j;

	// Basics
	uc = new UnitType();
	uc->mod = mod;
	uc->name = cfg_getstr(cfg_item, "name");
	uc->begin_health = (float)\cfg_getfloat(cfg_item, "health");
	uc->playable = cfg_getint(cfg_item, "playable");

	// 3D model
	char * tmp = cfg_getstr(cfg_item, "model");
	if (tmp == NULL) return NULL;
	uc->model = mod->getAssimpModel(tmp);
	if (uc->model == NULL) return NULL;

	// Col shape
	uc->col_shape = new btCapsuleShape(0.3f, 0.9f);
	
	// Params
	uc->params.max_speed = (float)cfg_getfloat(cfg_item, "max_speed");
	uc->params.melee_damage = cfg_getint(cfg_item, "melee_damage");
	uc->params.melee_delay = cfg_getint(cfg_item, "melee_delay");
	uc->params.melee_cooldown = cfg_getint(cfg_item, "melee_cooldown");
	uc->params.special_delay = cfg_getint(cfg_item, "special_delay");
	uc->params.special_cooldown = cfg_getint(cfg_item, "special_cooldown");

	// Animations
	int num_animations = cfg_size(cfg_item, "animation");
	for (j = 0; j < num_animations; j++) {
		cfg_t* cfg_anim = cfg_getnsec(cfg_item, "animation", j);
		
		UnitTypeAnimation* uta = new UnitTypeAnimation();
		
		uta->type = cfg_getint(cfg_anim, "type");
		uta->animation = cfg_getint(cfg_anim, "animation");
		uta->start_frame = cfg_getint(cfg_anim, "start_frame");
		uta->end_frame = cfg_getint(cfg_anim, "end_frame");
		uta->loop = (cfg_getint(cfg_anim, "loop") == 1);
		
		uc->animations.push_back(uta);
	}

	// Sounds
	int num_sounds = cfg_size(cfg_item, "sound");
	for (j = 0; j < num_sounds; j++) {
		cfg_t* cfg_sound = cfg_getnsec(cfg_item, "sound", j);
		
		UnitTypeSound* uts = new UnitTypeSound();
		
		uts->type = cfg_getint(cfg_sound, "type");
		
		char * tmp = cfg_getstr(cfg_sound, "sound");
		if (tmp == NULL) {
			delete(uts);
			return NULL;
		}
		uts->snd = mod->getSound(tmp);
		
		uc->sounds.push_back(uts);
	}

	// Spawn weapons
	int num_weapons = cfg_size(cfg_item, "spawn_weapons");
	for (j = 0; j < num_weapons; j++) {
		WeaponType * wt = mod->getWeaponType(cfg_getnstr(cfg_item, "spawn_weapons", j));
		
		if (wt != NULL) {
			uc->spawn_weapons.push_back(wt);
		}
	}

	// Special weapon
	tmp = cfg_getstr(cfg_item, "special_weapon");
	if (tmp != NULL) {
		uc->special_weapon = mod->getWeaponType(tmp);
	}

	return uc;
}


/**
* Returns a random sound which matches the specified type.
* If it can't find a sound for that type, return NULL
**/
Sound* UnitType::getSound(int type)
{
	unsigned int j = 0;
	unsigned int num = 0;
	
	if (this->sounds.size() == 0) return NULL;

	// Find out how many of this time exist
	for (j = 0; j < this->sounds.size(); j++) {
		if (this->sounds.at(j)->type == type) num++;
	}
	
	// Randomly choose one
	num = getRandom(0, num - 1);
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


/**
* Returns a random animation which matches the specified type.
* If it can't find an animation for that type, returns NULL
**/
UnitTypeAnimation* UnitType::getAnimation(int type)
{
	unsigned int j = 0;
	unsigned int num = 0;
	
	if (this->animations.size() == 0) return NULL;

	// Find out how many of this type exist
	for (j = 0; j < this->animations.size(); j++) {
		if (this->animations.at(j)->type == type) num++;
	}
	
	// Randomly choose one
	num = getRandom(0, num - 1);
	for (j = 0; j < this->animations.size(); j++) {
		if (this->animations.at(j)->type == type) {
			if (num == 0) {
				return this->animations.at(j);
			}
			num--;
		}
	}
	
	return NULL;
}


UnitType::UnitType()
{
	this->special_weapon = NULL;
	this->col_shape = NULL;
}

UnitType::~UnitType()
{
	delete(this->col_shape);
}


