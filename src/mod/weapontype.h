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
#define WEAPON_TYPE_FLAMETHROWER 3


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
		
		vector<WeaponTypeSound*> sounds;
		
	public:
		WeaponType();
		Sound* getSound(int type);

	public:
		virtual void doFire(Unit * u) = 0;
		
};


// Config file opts
extern cfg_opt_t weapontype_opts [];

// Item loading function handler
WeaponType* loadItemWeaponType(cfg_t* cfg_item, Mod* mod);



/**
* Weapons which fire by doing a raycast
* This is most common weapons (e.g. machineguns, pistols, etc)
**/
class WeaponRaycast : public WeaponType
{
	public:
		int angle_range;
		float range;
		float unit_damage;
		float wall_damage;

	public:
		virtual void doFire(Unit * u);
};


/**
* Weapons which affect the heightmap
**/
class WeaponDigdown : public WeaponType
{
	public:
		int radius;
		float depth;		// negative = go down, positive = go up

	public:
		virtual void doFire(Unit * u);
};


/**
* These do raycasts, but the effects are different to raycast weapons
* And the implementation might change down the line
**/
class WeaponFlamethrower : public WeaponType
{
	public:
		int angle_range;
		float range;
		float unit_damage;
		float wall_damage;

	public:
		virtual void doFire(Unit * u);
};

