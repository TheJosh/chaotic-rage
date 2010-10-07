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
AnimModel* loadAnimModel(cfg_t *cfg_model, Mod * mod);

/* Config file definition */
// MeshFrame
static cfg_opt_t meshframe_opts[] =
{
	CFG_INT((char*) "frame", 0, CFGF_NONE),
	CFG_STR((char*) "mesh", 0, CFGF_NONE),
	CFG_STR((char*) "texture", 0, CFGF_NONE),
	CFG_END()
};

// AnimModel
static cfg_opt_t model_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_INT((char*) "num_frames", 0, CFGF_NONE),
	CFG_SEC((char*) "meshframe", meshframe_opts, CFGF_MULTI),
	CFG_END()
};

// Main config
static cfg_opt_t opts[] =
{
	CFG_SEC((char*) "animmodel", model_opts, CFGF_MULTI),
	CFG_END()
};




AnimModel::AnimModel()
{
}


/**
* Loads the area types
**/
vector<AnimModel*> * loadAllAnimModels(Mod * mod)
{
	vector<AnimModel*> * models = new vector<AnimModel*>();
	
	char *buffer;
	cfg_t *cfg, *cfg_model;
	
	
	// Load + parse the config file
	buffer = mod->loadText("animmodels/animmodels.conf");
	if (buffer == NULL) {
		return NULL;
	}
	
	cfg = cfg_init(opts, CFGF_NONE);
	cfg_parse_buf(cfg, buffer);
	
	free(buffer);
	
	
	int num_types = cfg_size(cfg, "animmodel");
	if (num_types == 0) return NULL;
	
	// Process area type sections
	int j;
	for (j = 0; j < num_types; j++) {
		cfg_model = cfg_getnsec(cfg, "animmodel", j);
		
		AnimModel* am = loadAnimModel(cfg_model, mod);
		if (am == NULL) {
			cerr << "Bad animation model at index " << j << endl;
			return NULL;
		}
		
		models->push_back(am);
		am->id = models->size() - 1;
	}
	
	// If there was sprite errors, exit the game
	if (mod->st->render->wasLoadSpriteError()) {
		cerr << "Error loading animation models; game will now exit.\n";
		exit(1);
	}
	
	return models;
}


/**
* Loads a single area type
**/
AnimModel* loadAnimModel(cfg_t *cfg_model, Mod * mod)
{
	AnimModel* am;
	string filename;
	
	am = new AnimModel();
	am->name = cfg_getstr(cfg_model, "name");
	
	return am;
}


