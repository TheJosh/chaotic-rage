// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <list>
#include <confuse.h>
#include "../rage.h"


using namespace std;

class Mod;


class EffectType
{
	public:
		string name;
		CRC32 id;

	public:
		EffectType();
		~EffectType();

	private:
		EffectType(const EffectType& disabledCopyConstuctor);
};


// Config file opts
extern cfg_opt_t effecttype_opts [];

// Item loading function handler
EffectType* loadItemEffectType(cfg_t* cfg_item, Mod* mod);
