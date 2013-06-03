// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include "../rage.h"
#include "../mod/pickuptype.h"

using namespace std;


class PickupTypeAdjust;


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
	friend class NetClient;
	friend class NetServer;
	friend class AILogic;


	public:
		virtual EntityClass klass() const { return UNIT; }
		
	public:
		string name;
		Faction fac;
		unsigned int slot;		// zero for npcs, non-zero for players
		Vehicle* drive;

	protected:
		// TODO: Cleanup
		int health;
		int desired_angle_move;
		int angle_move;
		int desired_angle_aim;
		int angle_aim;
		
		btPairCachingGhostObject* ghost;
		btKinematicCharacterController* character;

		UnitWeapon* weapon;
		bool firing;
		vector<UnitWeapon*> avail_weapons;
		unsigned int curr_weapon_id;
		
		UnitType* uc;
		
		unsigned int animation_start;
		
		AnimPlay * anim;
		
		unsigned int remove_at;
		
		float old_x;
		float old_y;
		
		Object* lift_obj;
		Object* turret_obj;
		
		unsigned int melee_time;
		unsigned int melee_cooldown;
		
		Uint8 modifiers;
		UnitTypeSettings *uts;

		int weapon_sound;
		
		
	public:
		Unit(UnitType *uc, GameState *st, float x, float y, float z);
		virtual ~Unit();
		
	public:
		virtual AnimPlay* getAnimModel();
		virtual Sound* getSound();
		virtual void update(int delta);
		virtual btTransform &getTransform() { return ghost->getWorldTransform(); }
		virtual void setTransform(btTransform &t) { ghost->setWorldTransform(t); }

	public:
		Entity * infront(float range);
		bool onground();

		void beginFiring();
		void endFiring();
		void meleeAttack();
		void specialAttack();

		bool pickupWeapon(WeaponType* wt);
		bool pickupAmmo(WeaponType* wt);
		unsigned int getNumWeapons();
		UnitWeapon * getWeaponAt(unsigned int id);
		WeaponType * getWeaponTypeAt(unsigned int id);
		WeaponType * getWeaponTypeCurr();

		void setWeapon(int id);
		unsigned int getCurrentWeaponID();
		unsigned int getPrevWeaponID();
		unsigned int getNextWeaponID();
		int getBelt();
		int getMagazine();

		int getHealth();
		float getHealthPercent();
		virtual int takeDamage(int damage);
		
		UnitTypeSettings * getSettings();
		Uint8 getModifiers();
		void setModifiers(Uint8 modifiers);
		void addModifier(int modifier);
		void remModifier(int modifier);
		
		void applyPickupAdjust(PickupTypeAdjust* adj);
		void rollbackPickupAdjust(PickupTypeAdjust* adj);
		
	protected:
		void setState(int new_type);
		void doUse();
		void doLift();
		void doDrop();
		void emptySound();
		void enterVehicle(Vehicle *v);
		void leaveVehicle();
};
