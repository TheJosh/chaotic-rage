// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <SDL_mixer.h>
#include "../rage.h"

class Song
{
	public:
		// from data file
		std::string name;
		CRC32 id;

		Mix_Music * music;

	public:
		Song();
		~Song();
};


// Config file opts
extern cfg_opt_t song_opts [];

// Item loading function handler
Song* loadItemSong(cfg_t* cfg_item, Mod* mod);
