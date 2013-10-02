// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <vector>
#include <confuse.h>

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
class Mod;
class WeaponType;
class AssimpModel;
class btConvexShape;


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

class UnitParams
{
	public:
		float max_speed;        // meters/second
		int melee_damage;       // hit points
		int melee_delay;        // milliseconds
		int melee_cooldown;     // milliseconds
		int special_delay;      // milliseconds
		int special_cooldown;   // milliseconds
};

class UnitType
{
	friend UnitType* loadItemUnitType(cfg_t* cfg_item, Mod* mod);
	
	public:
		string name;
		CRC32 id;
		
		AssimpModel * model;
		btConvexShape* col_shape;
		int begin_health;
		vector<WeaponType*> spawn_weapons;
		WeaponType* special_weapon;
		int playable;
		UnitParams params;

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


