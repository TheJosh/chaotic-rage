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
Song* loadSong(cfg_t *cfg_song, Mod * mod);


/* Config file definition */
// Areatype section
static cfg_opt_t song_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_STR((char*) "file", 0, CFGF_NONE),
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
}


/**
* Loads the area types
**/
vector<Song*> * loadAllSongs(Mod * mod)
{
	vector<Song*> * songs = new vector<Song*>();
	
	char *buffer;
	cfg_t *cfg, *cfg_song;
	
	
	// Load + parse the config file
	buffer = mod->loadText("music/music.conf");
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
		
		Song* sg = loadSong(cfg_song, mod);
		if (sg == NULL) {
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
	Song* sg;
	string filename;
	
	filename = "music/";
	filename.append(cfg_getstr(cfg_song, "file"));
	
	SDL_RWops * rwops = mod->loadRWops(filename);
	
	sg = new Song();
	sg->name = cfg_getstr(cfg_song, "name");
	sg->music = Mix_LoadMUS_RW(rwops);
	
	if (sg->music == NULL) return NULL;
	
	return sg;
}


