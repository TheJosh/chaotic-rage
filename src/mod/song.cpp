// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include "../rage.h"
#include "mod.h"
#include "song.h"

using namespace std;


/**
* Config file opts
**/
cfg_opt_t song_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_STR((char*) "file", 0, CFGF_NONE),
	CFG_END()
};


/**
* Item loading function handler
**/
Song* loadItemSong(cfg_t* cfg_item, Mod* mod)
{
	Song* sg;
	string filename;
	
	sg = new Song();
	sg->name = cfg_getstr(cfg_item, "name");
	
	filename = "songs/";
	filename.append(cfg_getstr(cfg_item, "file"));
	SDL_RWops * rw = mod->loadRWops(filename);
	sg->music = Mix_LoadMUS_RW(rw, 0);		// todo: change?
	if (sg->music == NULL) return NULL;
	
	return sg;
}


Song::Song()
{
}
