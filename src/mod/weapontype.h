// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "../rage.h"

#define WEAPON_SOUND_BEGIN 1
#define WEAPON_SOUND_REPEAT 2
#define WEAPON_SOUND_END 3
#define WEAPON_SOUND_RELOAD 4
#define WEAPON_SOUND_EMPTY 5


class WeaponTypeSound
{
	public:
		int id;
		AudioPtr snd;
		int type;
};

class WeaponType		// TODO: Should this be renamed to just "Weapon"?
{
	public:
		std::string name;
		CRC32 id;

		std::string title;
		GameState * st;
		int type;

		unsigned int fire_delay;
		unsigned int reload_delay;
		bool continuous;
		int magazine_limit;
		int belt_limit;

		std::vector<WeaponTypeSound*> sounds;
		AssimpModel* model;
		AssimpModel* ammo_model;

		// Crosshair image and min/max size
		SpritePtr crosshair;
		int crosshair_min;
		int crosshair_max;

		std::vector<float> zoom_levels;

	public:
		WeaponType();
		virtual ~WeaponType() {}
		AudioPtr getSound(int type);

	public:
		/**
		* Called by the unit class when we should fire
		**/
		virtual void doFire(Unit *unit, btTransform &origin, float damage_multiplier) = 0;

		/**
		* Called by the associated ammo round to update stuff
		**/
		virtual void entityUpdate(AmmoRound *e, int delta) {}

};


// Config file opts
extern cfg_opt_t weapontype_opts [];

// Item loading function handler
WeaponType* loadItemWeaponType(cfg_t* cfg_item, Mod* mod);
