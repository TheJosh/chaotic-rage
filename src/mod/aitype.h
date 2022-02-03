// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <string>
#include "../types.h"


class Mod;


class AIType
{
	public:
		AIType();
		~AIType();

		std::string name;
		CRC32 id;
		std::string script;
};


// Config file opts
extern cfg_opt_t ai_opts [];

// Item loading function handler
AIType* loadItemAIType(cfg_t* cfg_item, Mod* mod);
