// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include "rage.h"

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
	
	snd = new Sound();
	snd->name = cfg_getstr(cfg_item, "name");
	
	filename = "sounds/";
	filename.append(cfg_getstr(cfg_item, "file"));
	snd->sound = mod->st->audio->loadSound(filename, mod);
	if (snd->sound == NULL) return NULL;
	
	return snd;
}


Sound::Sound()
{
}
