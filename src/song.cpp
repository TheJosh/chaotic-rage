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
Song* loadSong(cfg_t *cfg_Song, Mod * mod);


/* Config file definition */
// Areatype section
static cfg_opt_t song_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_STR((char*) "image", 0, CFGF_NONE),
	CFG_INT((char*) "stretch", 0, CFGF_NONE),		// 0 = tile, 1 = stretch
	CFG_INT((char*) "wall", 0, CFGF_NONE),			// 0 = ground, 1 = wall
	CFG_INT((char*) "ground_type", -1, CFGF_NONE),	// Ground to place underneath this wall
	CFG_END()
};

// Main config
static cfg_opt_t opts[] =
{
	CFG_SEC((char*) "song", song_opts, CFGF_MULTI),
	CFG_END()
};




Song::Song()
{
	this->surf = NULL;
	this->ground_type = NULL;
}


/**
* Loads the area types
**/
vector<Song*> * loadAllSongs(Mod * mod)
{
	vector<Song*> * songs = new vector<Song*>();
	
	char *buffer;
	cfg_t *cfg, *cfg_songs;
	
	
	// Load + parse the config file
	buffer = mod->loadText("areatypes/areatypes.conf");
	if (buffer == NULL) {
		return NULL;
	}
	
	cfg = cfg_init(opts, CFGF_NONE);
	cfg_parse_buf(cfg, buffer);
	
	free(buffer);
	
	
	int num_types = cfg_size(cfg, "song");
	if (num_types == 0) return NULL;
	
	// Process area type sections
	int j;
	for (j = 0; j < num_types; j++) {
		cfg_song = cfg_getnsec(cfg, "song", j);
		
		Song* sg = loadAreaType(cfg_song, mod);
		if (at == NULL) {
			cerr << "Bad song at index " << j << endl;
			return NULL;
		}
		
		songs->push_back(sg);
		sg->id = songs->size() - 1;
	}

	return songs;
}


/**
* Loads a single song
**/
Song* loadSong(cfg_t *cfg_song, Mod * mod)
{
	Song* at;
	string filename;
	
	sg = new Song();
	sg->surf = mod->st->render->loadSprite(filename.c_str(), mod);
	at->stretch = cfg_getint(cfg_areatype, "stretch");
	at->wall = cfg_getint(cfg_areatype, "wall");
	
	// TODO: move to after the mod has loaded
	//AreaType *ground = mod->getAreaType(cfg_getint(cfg_areatype, "ground_type"));
	//if (ground != NULL && ground->wall == 0) {
	//	at->ground_type = ground;
	//}
	
	return at;
}


