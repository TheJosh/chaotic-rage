// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include <map>
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
	CFG_FLOAT((char*) "px", 0, CFGF_NONE),
	CFG_FLOAT((char*) "py", 0, CFGF_NONE),
	CFG_FLOAT((char*) "pz", 0, CFGF_NONE),
	CFG_FLOAT((char*) "rx", 0, CFGF_NONE),
	CFG_FLOAT((char*) "ry", 0, CFGF_NONE),
	CFG_FLOAT((char*) "rz", 0, CFGF_NONE),
	CFG_FLOAT((char*) "sx", 1, CFGF_NONE),
	CFG_FLOAT((char*) "sy", 1, CFGF_NONE),
	CFG_FLOAT((char*) "sz", 1, CFGF_NONE),
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


// This is to save loading the same mesh into memory multiple times
// especially important for animation, etc.
static map <string, WavefrontObj *> loaded_meshes;
static map <string, SpritePtr> loaded_textures;


AnimModel::AnimModel()
{
}

MeshFrame::MeshFrame()
{
	mesh = NULL;
	texture = NULL;
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
	cfg_t *cfg_meshframe;
	
	am = new AnimModel();
	am->name = cfg_getstr(cfg_model, "name");
	am->num_frames = cfg_getint(cfg_model, "num_frames");
	
	int num = cfg_size(cfg_model, "meshframe");
	int j;
	for (j = 0; j < num; j++) {
		cfg_meshframe = cfg_getnsec(cfg_model, "meshframe", j);
		
		MeshFrame* mf = new MeshFrame();
		mf->frame = cfg_getint(cfg_meshframe, "frame");
		
		{
			string name = cfg_getstr(cfg_meshframe, "mesh");
			
			map<string, WavefrontObj *>::iterator it = loaded_meshes.find(name);
			if (it == loaded_meshes.end()) {
				/**
				* TODO: needs to use zzip!
				**/
				mf->mesh = loadObj("data/cr/animmodels/" + name + ".obj");
				
				if (mf->mesh == NULL) return NULL;
				
				loaded_meshes[name] = mf->mesh;
			} else {
				mf->mesh = it->second;
			}
		}
		
		{
			string name = cfg_getstr(cfg_meshframe, "texture");
			
			map<string, SpritePtr>::iterator it = loaded_textures.find(name);
			if (it == loaded_textures.end()) {
				mf->texture = mod->st->render->loadSprite("animmodels/" + name + ".png", mod);
				
				if (mf->texture == NULL) return NULL;
				
				loaded_textures[name] = mf->texture;
			} else {
				mf->texture = it->second;
			}
		}
		
		mf->texture_name = cfg_getstr(cfg_meshframe, "texture");
		
		mf->px = cfg_getfloat(cfg_meshframe, "px");
		mf->py = cfg_getfloat(cfg_meshframe, "py");
		mf->pz = cfg_getfloat(cfg_meshframe, "pz");
		mf->rx = cfg_getfloat(cfg_meshframe, "rx");
		mf->ry = cfg_getfloat(cfg_meshframe, "ry");
		mf->rz = cfg_getfloat(cfg_meshframe, "rz");
		mf->sx = cfg_getfloat(cfg_meshframe, "sx");
		mf->sy = cfg_getfloat(cfg_meshframe, "sy");
		mf->sz = cfg_getfloat(cfg_meshframe, "sz");
		
		am->meshframes.push_back(mf);
	}
	
	return am;
}


