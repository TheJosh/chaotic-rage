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
FloorType* loadFloorType(cfg_t *cfg_areatype, Mod * mod);


/* Config file definition */
// Areatype section
static cfg_opt_t areatype_opts[] =
{
	CFG_STR((char*) "name", (char*)"", CFGF_NONE),
	CFG_STR((char*) "image", (char*)"", CFGF_NONE),
	CFG_INT((char*) "stretch", 0, CFGF_NONE),		// 0 = tile, 1 = stretch
	CFG_END()
};

// Main config
static cfg_opt_t opts[] =
{
	CFG_SEC((char*) "floortype", areatype_opts, CFGF_MULTI),
	CFG_END()
};




FloorType::FloorType()
{
	this->texture = NULL;
}


/**
* Loads the area types
**/
vector<FloorType*> * loadAllFloorTypes(Mod * mod)
{
	vector<FloorType*> * areatypes = new vector<FloorType*>();
	
	char *buffer;
	cfg_t *cfg, *cfg_areatype;
	
	
	// Load + parse the config file
	buffer = mod->loadText("floortypes.conf");
	if (buffer == NULL) {
		return NULL;
	}
	
	cfg = cfg_init(opts, CFGF_NONE);
	cfg_parse_buf(cfg, buffer);
	
	free(buffer);
	
	
	int num_types = cfg_size(cfg, "floortype");
	if (num_types == 0) return NULL;
	
	// Process area type sections
	int j;
	for (j = 0; j < num_types; j++) {
		cfg_areatype = cfg_getnsec(cfg, "floortype", j);
		
		FloorType* at = loadFloorType(cfg_areatype, mod);
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
FloorType* loadFloorType(cfg_t *cfg_areatype, Mod * mod)
{
	FloorType* at;
	string filename;
	
	filename = "floortypes/";
	filename.append(cfg_getstr(cfg_areatype, "image"));
	filename.append("-fr0.png");
	
	at = new FloorType();
	at->name = cfg_getstr(cfg_areatype, "name");
	at->stretch = cfg_getint(cfg_areatype, "stretch");
	at->texture = mod->st->render->loadSprite(filename.c_str(), mod);
	
	return at;
}


