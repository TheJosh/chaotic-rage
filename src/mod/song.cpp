// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <confuse.h>
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
	char* tmp;

	sg = new Song();

	// Set name field
	tmp = cfg_getstr(cfg_item, "name");
	if (tmp == 0) {
		mod->setLoadErr("No value for field 'name'");
		delete(sg);
		return NULL;
	}
	sg->name = std::string(tmp);

	// Determine filename
	tmp = cfg_getstr(cfg_item, "file");
	if (tmp == 0) {
		mod->setLoadErr("No value for field 'file'");
		delete(sg);
		return NULL;
	}
	filename = "songs/";
	filename.append(tmp);

	// Read file
	SDL_RWops *rw = mod->loadRWops(filename);
	if (rw == 0) {
		mod->setLoadErr("Unable to read file '%s'", filename.c_str());
		delete(sg);
		return NULL;
	}

	// Load file
	sg->music = Mix_LoadMUS_RW(rw, 1);
	if (sg->music == NULL) {
		mod->setLoadErr("Unable to load file '%s'", filename.c_str());
		delete(sg);
		return NULL;
	}

	return sg;
}


Song::Song()
{
	this->music = NULL;
}

Song::~Song()
{
	Mix_FreeMusic(this->music);
}
