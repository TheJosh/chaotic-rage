// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include "../rage.h"

using namespace std;


#define UNIT_ANIM_STATIC 1
#define UNIT_ANIM_WALK 2
#define UNIT_ANIM_DEATH 3
#define UNIT_ANIM_SPAWN 4

#define UNIT_SOUND_STATIC 1
#define UNIT_SOUND_HIT 2
#define UNIT_SOUND_DEATH 3
#define UNIT_SOUND_SPAWN 4
#define UNIT_SOUND_ABILITY 5
#define UNIT_SOUND_CELEBRATE 6
#define UNIT_SOUND_FAIL 7


class UnitType;


class UnitTypeAnimation
{
	public:
		int type;
		int animation;
		int start_frame;
		int end_frame;
		bool loop;
};

class UnitTypeSound
{
	public:
		int id;
		Sound * snd;
		int type;
};

class UnitType
{
	friend UnitType* loadItemUnitType(cfg_t* cfg_item, Mod* mod);
	
	public:
		string name;
		AssimpModel * model;
		int id;
		int width, height;
		int begin_health;
		vector<WeaponType*> spawn_weapons;
		int playable;

		float max_speed;      // meters/second
		float accel;          // meters/second/second
		int turn;             // degrees/second
		int melee_damage;     // hit points
		int melee_delay;      // milliseconds
		int melee_cooldown;   // milliseconds

	private:
		vector<UnitTypeSound*> sounds;
		vector<UnitTypeAnimation*> animations;
		unsigned int max_frames;
		Mod * mod;
		
	public:
		UnitType();
		~UnitType();
		
	public:
		Sound* getSound(int type);
		UnitTypeAnimation* getAnimation(int type);
		
		Mod * getMod() { return this->mod; }
};


// Config file opts
extern cfg_opt_t unittype_opts [];

// Item loading function handler
UnitType* loadItemUnitType(cfg_t* cfg_item, Mod* mod);


