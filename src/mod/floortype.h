// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "../rage.h"

using namespace std;


class FloorType
{
	public:
		string name;
		CRC32 id;
		
		bool stretch;
		SpritePtr texture;
		
	public:
		FloorType();
};


// Config file opts
extern cfg_opt_t floortype_opts [];

// Item loading function handler
FloorType* loadItemFloorType(cfg_t* cfg_item, Mod* mod);

