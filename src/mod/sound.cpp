// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <string>
#include <confuse.h>
#include "../rage.h"
#include "../audio/audio.h"
#include "../game_state.h"
#include "../game_engine.h"
#include "mod.h"
#include "sound.h"

using namespace std;


/**
* Config file opts
**/
cfg_opt_t sound_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_STR((char*) "file", 0, CFGF_NONE),
	CFG_END()
};


/**
* Item loading function handler
**/
Sound* loadItemSound(cfg_t* cfg_item, Mod* mod)
{
	Sound* snd;
	string filename;
	char* tmp;

	snd = new Sound();

	// Set name field
	tmp = cfg_getstr(cfg_item, "name");
	if (tmp == 0) {
		mod->setLoadErr("No value for field 'name'");
		delete(snd);
		return NULL;
	}
	snd->name = std::string(tmp);

	// Determine filename
	tmp = cfg_getstr(cfg_item, "file");
	if (tmp == 0) {
		mod->setLoadErr("No value for field 'file'");
		delete(snd);
		return NULL;
	}
	filename = "sounds/";
	filename.append(tmp);

	// Read file
	snd->sound = GEng()->audio->loadSound(filename, mod);
	if (snd->sound == NULL) {
		mod->setLoadErr("Unable to load file '%s'", filename.c_str());
		delete(snd);
		return NULL;
	}

	return snd;
}


Sound::Sound()
{
}
