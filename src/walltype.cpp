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
WallType* loadWallType(cfg_t *cfg_walltype, Mod * mod);


/* Config file definition */
// Damage
static cfg_opt_t damage_opts[] =
{
	CFG_INT((char*) "health", 0, CFGF_NONE),
	CFG_STR((char*) "model", (char*)"", CFGF_NONE),
	CFG_END()
};

// Walltype section
static cfg_opt_t walltype_opts[] =
{
	CFG_STR((char*) "name", (char*)"", CFGF_NONE),
	CFG_STR((char*) "model", (char*)"", CFGF_NONE),
	CFG_STR_LIST((char*) "walk_sounds", 0, CFGF_NONE),
	CFG_SEC((char*) "damage", damage_opts, CFGF_MULTI),
	CFG_END()
};

// Main config
static cfg_opt_t opts[] =
{
	CFG_SEC((char*) "walltype", walltype_opts, CFGF_MULTI),
	CFG_END()
};




WallType::WallType()
{
	this->surf = NULL;
	this->ground_type = NULL;
}


/**
* Loads the area types
**/
vector<WallType*> * loadAllWallTypes(Mod * mod)
{
	vector<WallType*> * walltypes = new vector<WallType*>();
	
	char *buffer;
	cfg_t *cfg, *cfg_walltype;
	
	
	// Load + parse the config file
	buffer = mod->loadText("walltypes/walltypes.conf");
	if (buffer == NULL) {
		return NULL;
	}
	
	cfg = cfg_init(opts, CFGF_NONE);
	cfg_parse_buf(cfg, buffer);
	
	free(buffer);
	
	
	int num_types = cfg_size(cfg, "walltype");
	if (num_types == 0) return NULL;
	
	// Process area type sections
	int j;
	for (j = 0; j < num_types; j++) {
		cfg_walltype = cfg_getnsec(cfg, "walltype", j);
		
		WallType* wt = loadWallType(cfg_walltype, mod);
		if (wt == NULL) {
			cerr << "Bad area type at index " << j << endl;
			return NULL;
		}
		
		walltypes->push_back(wt);
		wt->id = walltypes->size() - 1;
	}
	
	// If there was sprite errors, exit the game
	if (mod->st->render->wasLoadSpriteError()) {
		cerr << "Error loading area types; game will now exit.\n";
		exit(1);
	}
	
	return walltypes;
}


/**
* Loads a single area type
**/
WallType* loadWallType(cfg_t *cfg_walltype, Mod * mod)
{
	WallType* wt;
	string filename;
	int size;
	
	wt = new WallType();
	wt->name = cfg_getstr(cfg_walltype, "name");
	wt->check_radius = 30;	//TODO: dynamic
	
	char * tmp = cfg_getstr(cfg_walltype, "model");
	if (tmp != NULL) {
		wt->model = mod->getAnimModel(tmp);
	}
	
	// Load damage states
	size = cfg_size(cfg_walltype, "damage");
	if (size == 0) {
		WallTypeDamage * dam = new WallTypeDamage();
		dam->health = 0;
		dam->model = mod->getAnimModel("null");
		wt->damage_models.push_back(dam);
		
	} else {
		for (int j = 0; j < size; j++) {
			cfg_t *cfg_damage = cfg_getnsec(cfg_walltype, "damage", j);
			
			char * tmp = cfg_getstr(cfg_damage, "model");
			if (tmp == NULL) return NULL;
			
			WallTypeDamage * dam = new WallTypeDamage();
			
			dam->health = cfg_getint(cfg_damage, "health");
			dam->model = mod->getAnimModel(tmp);
			
			wt->damage_models.push_back(dam);
		}
	}
	
	// Load walk sounds
	size = cfg_size(cfg_walltype, "walk_sounds");
	for (int j = 0; j < size; j++) {
		filename = "walltypes/";
		filename.append(cfg_getnstr(cfg_walltype, "walk_sounds", j));
		filename.append(".wav");
		
		AudioPtr sound = mod->st->audio->loadSound(filename, mod);
		if (sound == NULL) return NULL;
		wt->walk_sounds.push_back(sound);
	}
	
	// TODO: move to after the mod has loaded
	//WallType *ground = mod->getWallType(cfg_getint(cfg_walltype, "ground_type"));
	//if (ground != NULL && ground->wall == 0) {
	//	wt->ground_type = ground;
	//}
	
	return wt;
}


