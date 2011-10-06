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
		// TODO: Cleanup
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
		unsigned int weapon_fire_time;
		
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
		
		Object* curr_obj;
		Object* lift_obj;
		Object* drive_obj;
		Object* turret_obj;

		Unit* closest;
		unsigned int melee_time;
		unsigned int melee_cooldown;

		int weapon_sound;

	public:
		Unit(UnitType *uc, GameState *st);
		virtual ~Unit();
		
	public:
		virtual AnimPlay* getAnimModel();
		virtual Sound* getSound();
		virtual void update(int delta) = 0;
		virtual void hasBeenHit(CollideBox * ours, CollideBox * theirs);
		virtual void entityClose(Entity * e, float dist);

	public:
		void beginFiring();
		void endFiring();
		void meleeAttack();
		void specialAttack();

		int pickupWeapon(WeaponType* wt);
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
		void doUse();
		void doLift();
		void doDrop();
};
