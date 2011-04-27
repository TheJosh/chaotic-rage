// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include "rage.h"

using namespace std;


/* Functions */
ParticleType* loadParticleType(cfg_t *cfg_particletype, Mod * mod);
ParticleGenType* loadParticleGenType(cfg_t *cfg_generatortype, Mod * mod);

extern cfg_opt_t g_action_opts;

/* Config file definition */
// Particle section
static cfg_opt_t particletype_opts[] =
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
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_INT((char*) "type", 0, CFGF_NONE),
	CFG_INT((char*) "angle_range", 0, CFGF_NONE),
	CFG_INT((char*) "rate", 0, CFGF_NONE),
	CFG_INT((char*) "delay", 0, CFGF_NONE),
	CFG_INT((char*) "time", 0, CFGF_NONE),
	CFG_INT((char*) "offset", 0, CFGF_NONE),
	CFG_END()
};

// Particle generator section
static cfg_opt_t generatortype_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_SEC((char*) "spew", spew_opts, CFGF_MULTI),
	CFG_END()
};

// Main config - particletypes.conf
static cfg_opt_t opts_particles[] =
{
	CFG_SEC((char*) "particle", particletype_opts, CFGF_MULTI),
	CFG_END()
};

// Main config - particlegenerators.conf
static cfg_opt_t opts_generators[] =
{
	CFG_SEC((char*) "generator", generatortype_opts, CFGF_MULTI),
	CFG_END()
};



ParticleType::ParticleType()
{
}

ParticleType::~ParticleType()
{
}


/**
* Loads the area types
**/
vector<ParticleType*> * loadAllParticleTypes(Mod * mod)
{
	vector<ParticleType*> * particletypes = new vector<ParticleType*>();
	
	char *buffer;
	cfg_t *cfg, *cfg_particletype;
	
	
	// Load + parse the config file
	buffer = mod->loadText("particletypes/particletypes.conf");
	if (buffer == NULL) {
		return NULL;
	}
	
	cfg = cfg_init(opts_particles, CFGF_NONE);
	cfg_parse_buf(cfg, buffer);
	
	free(buffer);
	
	
	int num_types = cfg_size(cfg, "particle");
	if (num_types == 0) return NULL;
	
	// Process particle type sections
	for (int j = 0; j < num_types; j++) {
		cfg_particletype = cfg_getnsec(cfg, "particle", j);
		
		ParticleType* pt = loadParticleType(cfg_particletype, mod);
		if (pt == NULL) {
			cerr << "Bad particle type at index " << j << endl;
			return NULL;
		}
		
		particletypes->push_back(pt);
		pt->id = particletypes->size() - 1;
	}
	
	
	// If there was sprite errors, exit the game
	if (mod->st->render->wasLoadSpriteError()) {
		cerr << "Error loading particle types; game will now exit.\n";
		exit(1);
	}
	
	return particletypes;
}

/**
* Loads particle generators
**/
vector<ParticleGenType*> * loadAllParticleGenTypes(Mod * mod)
{
	vector<ParticleGenType*> * generatortypes = new vector<ParticleGenType*>();
	
	char *buffer;
	cfg_t *cfg, *cfg_generatortype;
	
	// Load + parse the config file
	buffer = mod->loadText("particletypes/particlegenerators.conf");
	if (buffer == NULL) {
		return NULL;
	}
	
	cfg = cfg_init(opts_generators, CFGF_NONE);
	cfg_parse_buf(cfg, buffer);
	
	free(buffer);
	
	
	// Process generator type sections
	int num_types = cfg_size(cfg, "generator");
	if (num_types == 0) return NULL;
	
	for (int j = 0; j < num_types; j++) {
		cfg_generatortype = cfg_getnsec(cfg, "generator", j);
		
		ParticleGenType* gt = loadParticleGenType(cfg_generatortype, mod);
		if (gt == NULL) {
			cerr << "Bad particle generator type at index " << j << endl;
			return NULL;
		}
		
		generatortypes->push_back(gt);
		gt->id = generatortypes->size() - 1;
	}
	
	return generatortypes;
}


/**
* Loads a single particle type
**/
ParticleType* loadParticleType(cfg_t *cfg_particletype, Mod * mod)
{
	ParticleType* pt;
	
	if (cfg_size(cfg_particletype, "model") == 0) return NULL;
	if (cfg_getint(cfg_particletype, "age") == 0) return NULL;
	
	// Load settings
	pt = new ParticleType();
	pt->max_speed = cfg_getrange(cfg_particletype, "max_speed");
	pt->begin_speed = cfg_getrange(cfg_particletype, "begin_speed");
	pt->accel = cfg_getrange(cfg_particletype, "accel");
	pt->age = cfg_getrange(cfg_particletype, "age");
	pt->unit_damage = cfg_getrange(cfg_particletype, "unit_damage");
	pt->wall_damage = cfg_getrange(cfg_particletype, "wall_damage");
	pt->unit_hits = cfg_getrange(cfg_particletype, "unit_hits");
	pt->wall_hits = cfg_getrange(cfg_particletype, "wall_hits");
	
	char * tmp = cfg_getstr(cfg_particletype, "model");
	if (tmp == NULL) return NULL;
	pt->model = mod->getAnimModel(tmp);
	if (pt->model == NULL) return NULL;
	
	return pt;
}


/**
* Loads a single particle generator type
**/
ParticleGenType* loadParticleGenType(cfg_t *cfg_generatortype, Mod * mod)
{
	ParticleGenType* gt;
	cfg_t *cfg_spew;
	int j;
	
	if (cfg_size(cfg_generatortype, "name") == 0) return NULL;
	if (cfg_size(cfg_generatortype, "spew") == 0) return NULL;
	
	gt = new ParticleGenType();
	
	int num_types = cfg_size(cfg_generatortype, "spew");
	for (j = 0; j < num_types; j++) {
		cfg_spew = cfg_getnsec(cfg_generatortype, "spew", j);
		
		if (cfg_getint(cfg_spew, "angle_range") == 0) return NULL;
		if (cfg_getint(cfg_spew, "rate") == 0) return NULL;
		
		GenSpew* spew = new GenSpew();
		spew->pt = mod->getParticleType(cfg_getint(cfg_spew, "type"));
		spew->angle_range = cfg_getint(cfg_spew, "angle_range");
		spew->rate = cfg_getint(cfg_spew, "rate");
		spew->delay = cfg_getint(cfg_spew, "delay");
		spew->time = cfg_getint(cfg_spew, "time");
		spew->offset = cfg_getint(cfg_spew, "offset");
		
		gt->spewers.push_back(spew);
	}
	
	return gt;
}


