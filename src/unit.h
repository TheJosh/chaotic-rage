// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class UnitWeapon {
	friend class Unit;
	private:
		WeaponType* wt;
		int magazine;
		int belt;
		unsigned int next_use;		// time after which the weapon can be used again
		bool reloading;
};

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
		
		UnitWeapon* weapon;
		bool firing;
		vector<UnitWeapon*> avail_weapons;
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
		
		Object* curr_obj;
		Object* lift_obj;
		Object* drive_obj;
		Object* turret_obj;
		
		Unit* closest;
		unsigned int melee_time;
		unsigned int melee_cooldown;
		
		int weapon_sound;
		
		
	public:
		Unit(UnitType *uc, GameState *st, float x, float y, float z);
		virtual ~Unit();
		
	public:
		virtual AnimPlay* getAnimModel();
		virtual Sound* getSound();
		virtual void update(int delta) = 0;
		virtual void hasBeenHit(Entity * that);
		virtual void entityClose(Entity * e, float dist);

	public:
		Entity * infront(float range);
		
		void beginFiring();
		void endFiring();
		void meleeAttack();
		void specialAttack();
		
		bool pickupWeapon(WeaponType* wt);
		bool pickupAmmo(WeaponType* wt);
		unsigned int getNumWeapons();
		UnitWeapon * getWeaponAt(unsigned int id);
		WeaponType * getWeaponTypeAt(unsigned int id);
		
		void setWeapon(int id);
		unsigned int getCurrentWeaponID();
		unsigned int getPrevWeaponID();
		unsigned int getNextWeaponID();
		int getBelt();
		int getMagazine();
		int getHealth();
		
		virtual int takeDamage(int damage);
		
	protected:
		void update(int delta, UnitTypeSettings *ucs);
		void setState(int new_type);
		void doUse();
		void doLift();
		void doDrop();
};
