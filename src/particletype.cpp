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
// Particle section
cfg_opt_t particletype_opts[] =
{
	CFG_STR((char*) "name", (char*)"", CFGF_NONE),
	CFG_STR((char*) "model", (char*)"", CFGF_NONE),
	CFG_INT_LIST((char*) "max_speed", 0, CFGF_NONE),
	CFG_INT_LIST((char*) "begin_speed", 0, CFGF_NONE),
	CFG_INT_LIST((char*) "accel", 0, CFGF_NONE),
	CFG_INT_LIST((char*) "age", 0, CFGF_NONE),
	CFG_INT_LIST((char*) "unit_damage", 0, CFGF_NONE),
	CFG_INT_LIST((char*) "wall_damage", 0, CFGF_NONE),
	CFG_INT_LIST((char*) "unit_hits", 0, CFGF_NONE),
	CFG_INT_LIST((char*) "wall_hits", 0, CFGF_NONE),
	CFG_END()
};

// Particle generator spewage
static cfg_opt_t spew_opts[] =
{
	CFG_STR((char*) "type", (char*)"", CFGF_NONE),
	CFG_INT((char*) "angle_range", 0, CFGF_NONE),
	CFG_INT((char*) "rate", 0, CFGF_NONE),
	CFG_INT((char*) "delay", 0, CFGF_NONE),
	CFG_INT((char*) "time", 0, CFGF_NONE),
	CFG_INT((char*) "offset", 0, CFGF_NONE),
	CFG_END()
};

// Particle generator section
cfg_opt_t generatortype_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_SEC((char*) "spew", spew_opts, CFGF_MULTI),
	CFG_END()
};



/**
* Loads a single particle type
**/
ParticleType* loadItemParticleType(cfg_t* cfg_item, Mod* mod)
{
	ParticleType* pt;
	
	if (cfg_size(cfg_item, "name") == 0) return NULL;
	if (cfg_size(cfg_item, "model") == 0) return NULL;
	if (cfg_getint(cfg_item, "age") == 0) return NULL;
	
	// Load settings
	pt = new ParticleType();
	pt->name = cfg_getstr(cfg_item, "name");
	pt->max_speed = cfg_getrange(cfg_item, "max_speed");
	pt->begin_speed = cfg_getrange(cfg_item, "begin_speed");
	pt->accel = cfg_getrange(cfg_item, "accel");
	pt->age = cfg_getrange(cfg_item, "age");
	pt->unit_damage = cfg_getrange(cfg_item, "unit_damage");
	pt->wall_damage = cfg_getrange(cfg_item, "wall_damage");
	pt->unit_hits = cfg_getrange(cfg_item, "unit_hits");
	pt->wall_hits = cfg_getrange(cfg_item, "wall_hits");
	
	char * tmp = cfg_getstr(cfg_item, "model");
	if (tmp == NULL) return NULL;
	pt->model = mod->getAnimModel(tmp);
	if (pt->model == NULL) return NULL;
	
	return pt;
}


/**
* Loads a single particle generator type
**/
ParticleGenType* loadItemParticleGenType(cfg_t* cfg_item, Mod* mod)
{
	ParticleGenType* gt;
	cfg_t *cfg_spew;
	int j;
	
	if (cfg_size(cfg_item, "name") == 0) return NULL;
	if (cfg_size(cfg_item, "spew") == 0) return NULL;
	
	gt = new ParticleGenType();
	gt->name = cfg_getstr(cfg_item, "name");


	// Spewers
	int num_spew = cfg_size(cfg_item, "spew");

	for (j = 0; j < num_spew; j++) {
		cfg_spew = cfg_getnsec(cfg_item, "spew", j);
		
		if (cfg_size(cfg_spew, "type") == 0) return NULL;
		if (cfg_getint(cfg_spew, "angle_range") == 0) return NULL;
		if (cfg_getint(cfg_spew, "rate") == 0) return NULL;
		
		GenSpew* spew = new GenSpew();
		spew->angle_range = cfg_getint(cfg_spew, "angle_range");
		spew->rate = cfg_getint(cfg_spew, "rate");
		spew->delay = cfg_getint(cfg_spew, "delay");
		spew->time = cfg_getint(cfg_spew, "time");
		spew->offset = cfg_getint(cfg_spew, "offset");
		
		char * tmp = cfg_getstr(cfg_spew, "type");
		if (tmp == NULL) return NULL;
		spew->pt = mod->getParticleType(tmp);
		if (spew->pt == NULL) return NULL;

		gt->spewers.push_back(spew);
	}
	

	return gt;
}


