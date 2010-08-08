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
AreaType* loadAreaType(cfg_t *cfg_areatype, Mod * mod);


/* Config file definition */
// Areatype section
static cfg_opt_t areatype_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_STR((char*) "image", 0, CFGF_NONE),
	CFG_INT((char*) "stretch", 0, CFGF_NONE),		// 0 = tile, 1 = stretch
	CFG_INT((char*) "wall", 0, CFGF_NONE),			// 0 = ground, 1 = wall
	CFG_INT((char*) "ground_type", -1, CFGF_NONE),	// Ground to place underneath this wall
	CFG_STR_LIST((char*) "walk_sounds", 0, CFGF_NONE),
	CFG_END()
};

// Main config
static cfg_opt_t opts[] =
{
	CFG_SEC((char*) "areatype", areatype_opts, CFGF_MULTI),
	CFG_END()
};




AreaType::AreaType()
{
	this->surf = NULL;
	this->ground_type = NULL;
}


/**
* Loads the area types
**/
vector<AreaType*> * loadAllAreaTypes(Mod * mod)
{
	vector<AreaType*> * areatypes = new vector<AreaType*>();
	
	char *buffer;
	cfg_t *cfg, *cfg_areatype;
	
	
	// Load + parse the config file
	buffer = mod->loadText("areatypes/areatypes.conf");
	if (buffer == NULL) {
		return NULL;
	}
	
	cfg = cfg_init(opts, CFGF_NONE);
	cfg_parse_buf(cfg, buffer);
	
	free(buffer);
	
	
	int num_types = cfg_size(cfg, "areatype");
	if (num_types == 0) return NULL;
	
	// Process area type sections
	int j;
	for (j = 0; j < num_types; j++) {
		cfg_areatype = cfg_getnsec(cfg, "areatype", j);
		
		AreaType* at = loadAreaType(cfg_areatype, mod);
		if (at == NULL) {
			cerr << "Bad area type at index " << j << endl;
			return NULL;
		}
		
		areatypes->push_back(at);
		at->id = areatypes->size() - 1;
	}
	
	// If there was sprite errors, exit the game
	if (mod->st->render->wasLoadSpriteError()) {
		cerr << "Error loading area types; game will now exit.\n";
		exit(1);
	}
	
	return areatypes;
}


/**
* Loads a single area type
**/
AreaType* loadAreaType(cfg_t *cfg_areatype, Mod * mod)
{
	AreaType* at;
	string filename;
	
	filename = "areatypes/";
	filename.append(cfg_getstr(cfg_areatype, "image"));
	filename.append("-fr0.png");
	
	at = new AreaType();
	at->surf = mod->st->render->loadSprite(filename.c_str(), mod);
	at->stretch = cfg_getint(cfg_areatype, "stretch");
	at->wall = cfg_getint(cfg_areatype, "wall");
	
	// Load walk sounds
	int size = cfg_size(cfg_areatype, "walk_sounds");
	for (int j = 0; j < size; j++) {
		filename = "areatypes/";
		filename.append(cfg_getnstr(cfg_areatype, "walk_sounds", j));
		filename.append(".wav");
		
		AudioPtr sound = mod->st->audio->loadSound(filename, mod);
		if (sound == NULL) return NULL;
		at->walk_sounds.push_back(sound);
	}
	
	// TODO: move to after the mod has loaded
	//AreaType *ground = mod->getAreaType(cfg_getint(cfg_areatype, "ground_type"));
	//if (ground != NULL && ground->wall == 0) {
	//	at->ground_type = ground;
	//}
	
	return at;
}


