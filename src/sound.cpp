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
Sound* loadSound(cfg_t *cfg_sound, Mod * mod);


/* Config file definition */
// Areatype section
static cfg_opt_t sound_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_STR((char*) "file", 0, CFGF_NONE),
	CFG_END()
};

// Main config
static cfg_opt_t opts[] =
{
	CFG_SEC((char*) "sound", sound_opts, CFGF_MULTI),
	CFG_END()
};




Sound::Sound()
{
}


/**
* Loads the area types
**/
vector<Sound*> * loadAllSounds(Mod * mod)
{
	vector<Sound*> * sounds = new vector<Sound*>();
	
	char *buffer;
	cfg_t *cfg, *cfg_sound;
	
	
	// Load + parse the config file
	buffer = mod->loadText("sounds.conf");
	if (buffer == NULL) {
		return NULL;
	}
	
	cfg = cfg_init(opts, CFGF_NONE);
	cfg_parse_buf(cfg, buffer);
	
	free(buffer);
	
	
	int num_types = cfg_size(cfg, "sound");
	if (num_types == 0) return NULL;
	
	// Process area type sections
	int j;
	for (j = 0; j < num_types; j++) {
		cfg_sound = cfg_getnsec(cfg, "sound", j);
		
		Sound* snd = loadSound(cfg_sound, mod);
		if (snd == NULL) {
			cerr << "Bad sound at index " << j << endl;
			return NULL;
		}
		
		sounds->push_back(snd);
		snd->id = sounds->size() - 1;
	}

	return sounds;
}


/**
* Loads a single sound
**/
Sound* loadSound(cfg_t *cfg_sound, Mod * mod)
{
	Sound* snd;
	string filename;
	
	filename = "sounds/";
	filename.append(cfg_getstr(cfg_sound, "file"));
	
	snd = new Sound();
	snd->sound = mod->st->audio->loadSound(filename, mod);
	snd->name = cfg_getstr(cfg_sound, "name");
	
	if (snd->sound == NULL) return NULL;
	
	return snd;
}


