// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "../rage.h"


enum WeaponAttachmentLocation
{
	WPATT_TOP,
	WPATT_LEFT,
	WPATT_RIGHT,
	WPATT_BARREL,
	NUM_WPATT,
};


class WeaponAttachment
{
	public:
		std::string name;
		CRC32 id;
		btVector3 loc[NUM_WPATT];
		
	public:
		bool canAttach(WeaponAttachmentLocation loc);
};


// Config file opts
extern cfg_opt_t weaponattachment_opts[];

// Item loading function handler
WeaponAttachment* loadItemWeaponAttachment(cfg_t* cfg_item, Mod* mod);
