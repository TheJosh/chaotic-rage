// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "entity.h"
#include "../mod/unittype.h"


using namespace std;


class PickupTypeAdjust;
class UnitParams;
class Entity;
class WeaponType;
class Unit;
class PickupType;
class Vehicle;
class Object;
class UnitType;
class btPairCachingGhostObject;
class btCRKinematicCharacterController;


class UnitWeapon {
	friend class Unit;
	private:
		WeaponType* wt;
		int magazine;
		int belt;
		unsigned int next_use;		// time after which the weapon can be used again
		bool reloading;
};

class UnitPickup {
	public:
		Unit* u;
		PickupType* pt;
		unsigned int end_time;
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
		UnitType* uc;
		UnitParams params;

		float health;
		unsigned int remove_at;

		AnimPlay * anim;
		int weapon_sound;

		btPairCachingGhostObject* ghost;
		btCRKinematicCharacterController* character;

		Object* lift_obj;
		btVector3 force;

		UnitWeapon* weapon;
		bool firing;
		vector<UnitWeapon*> avail_weapons;
		unsigned int curr_weapon_id;

		unsigned int melee_cooldown;

		bool special_firing;
		unsigned int special_time;
		unsigned int special_cooldown;

		list<UnitPickup> pickups;

	public:
		Unit(UnitType *uc, GameState *st, float x, float y, float z, Faction fac);
		virtual ~Unit();
		
	public:
		virtual AnimPlay* getAnimModel();
		virtual Sound* getSound();
		virtual void update(int delta);
		virtual btTransform &getTransform();
		virtual void setTransform(btTransform &t);

	public:
		Entity * infront(float range);
		Entity * raytest(btMatrix3x3 &direction, float range);
		bool onground();

		void beginFiring();
		void endFiring();
		void meleeAttack();
		void meleeAttack(btMatrix3x3 &direction);
		void specialAttack();
		void endSpecialAttack();

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

		float getHealth();
		float getHealthPercent();
		virtual int takeDamage(float damage);
		
		void applyPickupAdjust(PickupTypeAdjust* adj);
		void rollbackPickupAdjust(PickupTypeAdjust* adj);
		void applyForce(btVector3 &force);
		
		UnitType* getUnitType() { return this->uc; }
		UnitParams* getParams() { return &this->params; }
		
	protected:
		void setState(int new_type);
		void doUse();
		void doLift();
		void doDrop();
		void emptySound();
		void enterVehicle(Vehicle *v);
		void leaveVehicle();
};
