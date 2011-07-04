// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class Unit : public Entity
{
	public:
		virtual EntityClass klass() const { return UNIT; }
		
	public:
		string name;
		Faction fac;
		int slot;
		
	protected:
		float speed;
		int health;
		int desired_angle_move;
		int angle_move;
		int desired_angle_aim;
		int angle_aim;
		
		WeaponType* weapon;
		ParticleGenerator* weapon_gen;
		bool firing;
		vector<WeaponType*> avail_weapons;
		unsigned int curr_weapon_id;
		
		UnitType* uc;
		UnitTypeState* current_state;
		int current_state_type;
		
		UnitTypeState* walk_state;
		unsigned int walk_time;
		
		unsigned int animation_start;
		
		AnimPlay * anim;
		
		unsigned int remove_at;
		
		float old_x;
		float old_y;
		
		CollideBox* cb;
		
	public:
		Unit(UnitType *uc, GameState *st);
		virtual ~Unit();
		
	public:
		virtual AnimPlay* getAnimModel();
		virtual Sound* getSound();
		virtual void update(int delta) = 0;
		virtual void hasBeenHit(CollideBox * ours, CollideBox * theirs);
		
	public:
		void beginFiring();
		void endFiring();
		
		void pickupWeapon(WeaponType* wt);
		unsigned int getNumWeapons();
		WeaponType * getWeaponAt(unsigned int id);
		
		void setWeapon(int id);
		unsigned int getCurrentWeaponID();
		unsigned int getPrevWeaponID();
		unsigned int getNextWeaponID();
		
		virtual int takeDamage(int damage);
		
	protected:
		void update(int delta, UnitTypeSettings *ucs);
		void setState(int new_type);
};
