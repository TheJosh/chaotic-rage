// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "../rage.h"

using namespace std;


class GameType
{
	public:
		string name;
		CRC32 id;
		
		string script;
		string title;
		
	public:
		GameType();
		
	public:
		bool operator < (const GameType& other) const
		{
			return (title < other.title);
		}
};


// Config file opts
extern cfg_opt_t gametype_opts [];

// Item loading function handler
GameType* loadItemGameType(cfg_t* cfg_item, Mod* mod);
