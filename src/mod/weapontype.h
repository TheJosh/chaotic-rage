// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "../rage.h"

using namespace std;


#define WEAPON_SOUND_BEGIN 1
#define WEAPON_SOUND_REPEAT 2
#define WEAPON_SOUND_END 3
#define WEAPON_SOUND_RELOAD 4
#define WEAPON_SOUND_EMPTY 5

#define WEAPON_TYPE_RAYCAST 1
#define WEAPON_TYPE_DIGDOWN 2


class WeaponTypeSound
{
	public:
		int id;
		Sound * snd;
		int type;
};

class WeaponType		// TODO: Should this be renamed to just "Weapon"?
{
	public:
		int id;
		string name;
		string title;
		GameState * st;
		
		unsigned int fire_delay;
		unsigned int reload_delay;
		bool continuous;
		int magazine_limit;
		int belt_limit;
		float unit_damage;
		float wall_damage;
		
		vector<WeaponTypeSound*> sounds;
		
	public:
		WeaponType();
		Sound* getSound(int type);

	public:
		virtual void doFire(Unit * u) = 0;
		
};


/**
* Weapons which "fire" by doing a raycast
* This is most common weapons (e.g. machineguns, pistols, etc)
**/
class WeaponRaycast : public WeaponType
{
	public:
		int angle_range;
		float range;

	public:
		virtual void doFire(Unit * u);
};


/**
* Weapons which "fire" by doing a raycast
* This is most common weapons (e.g. machineguns, pistols, etc)
**/
class WeaponDigdown : public WeaponType
{
	public:
		float radius;

	public:
		virtual void doFire(Unit * u);
};


// Config file opts
extern cfg_opt_t weapontype_opts [];

// Item loading function handler
WeaponType* loadItemWeaponType(cfg_t* cfg_item, Mod* mod);

