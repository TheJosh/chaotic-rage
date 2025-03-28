// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <list>
#include <string>
#include <vector>
#include "../../game_state.h"
#include "../../mod/unittype.h"
#include "../../mod/weaponattachment.h"
#include "../entity.h"


class AnimPlay;
class Object;
class PickupType;
class PickupTypeAdjust;
class Sound;
class Unit;
class UnitPickup;
class UnitWeapon;
class Vehicle;
class WeaponType;
class btCharacterControllerInterface;
class btCollisionObject;
class btTransform;
class btVector3;
class btQuaternion;


using namespace std;


/**
* Base class for all units - Players and NPCs
**/
class Unit : public Entity
{
	friend class NetClient;
	friend class NetServer;


	public:
		virtual EntityClass klass() const { return UNIT; }

	public:
		string name;
		Faction fac;
		unsigned int slot;		// zero for npcs, non-zero for players
		
	protected:
		// Type
		UnitType* uc;

		// Parameters are copied from the type
		// Might also be adjusted by powerups
		UnitParams params;

		btQuaternion aim_dir;

		// When this reaches zero, the unit dies
		float health;
		unsigned int last_hit;

		Vehicle* drive;

		// 3D model
		AnimPlay* anim;

		// Current firing sound
		int weapon_sound;

		// Countdown until idle sound
		unsigned int idle_sound_time;

		// Physics bits
		btCollisionObject* ghost;
		btCharacterControllerInterface* char_ctlr;

		// The object we're lifting
		Entity* lift_obj;

		// External forces (e.g. black holes)
		btVector3 force;

		// The current weapon
		UnitWeapon* weapon;
		unsigned int curr_weapon_id;
		unsigned int weapon_zoom_level;

		// Are we firing?
		bool firing;

		// All of the weapons
		vector<UnitWeapon*> avail_weapons;

		// How long until we can melee again
		unsigned int melee_cooldown;

		// Is the special weapon firing?
		bool special_firing;

		// When will the special be finished?
		unsigned int special_time;
		unsigned int special_cooldown;

		// Currently applied pickups
		list<UnitPickup> pickups;

		// A weapon specified by a powerup
		WeaponType* powerup_weapon;

		// A message to be shown while the powerup is active
		string powerup_message;

		// Not active until fully spawned
		bool active;

	public:
		Unit(UnitType *ut, GameState *st, Faction fac, float x, float z);
		Unit(UnitType *ut, GameState *st, Faction fac, float x, float y, float z);
		Unit(UnitType *ut, GameState *st, Faction fac, btTransform & loc);
		void init(UnitType *ut, GameState *st, Faction fac, btTransform & loc);
		virtual ~Unit();

	public:
		// Update every frame
		virtual void update(float delta);
		virtual void physicsUpdate(float delta);

		// The location and rotation in the universe
		virtual const btTransform &getTransform() const;
		virtual void setTransform(btTransform &t);

	public:
		// Ray tests
		Entity * infront(float range);
		Entity * raytest(btMatrix3x3 &direction, float range);
		bool onground();

		// Traditional weapons
		void beginFiring();
		void endFiring();

		// Melee
		void meleeAttack();
		void meleeAttack(btMatrix3x3 &direction);

		// Special attack
		void specialAttack();
		void endSpecialAttack();

		// Pick up weapons
		bool pickupWeapon(WeaponType* wt);
		bool pickupAmmo(WeaponType* wt);

		// Weapon list
		unsigned int getNumWeapons();
		UnitWeapon* getWeaponAt(unsigned int id);
		WeaponType* getWeaponTypeAt(unsigned int id);
		unsigned int getPrevWeaponID();
		unsigned int getNextWeaponID();

		// Current weapon
		WeaponType* getWeaponTypeCurr();
		unsigned int getCurrentWeaponID();

		// Set the weapon
		void setWeapon(int id);

		void weaponAttach(WeaponAttachmentLocation loc, WeaponAttachment* att);
		void weaponDetatch(WeaponAttachmentLocation loc);
		WeaponAttachment* getAttachment(WeaponAttachmentLocation loc);

		// Ammo and zoom level
		int getBelt();
		int getMagazine();
		float getWeaponZoom();
		void reload();

		// Health and death
		float getHealth();
		float getHealthPercent();
		virtual void takeDamage(float damage);
		virtual void die();

		// Vehicles
		bool isInVehicle() { return (drive != NULL); }

		// Pickup adjustments (temporary and permanant)
		void applyPickupAdjust(PickupTypeAdjust* adj);
		void rollbackPickupAdjust(PickupTypeAdjust* adj);
		void addActivePickup(PickupType* pt);
		bool hasActivePickup(PickupType* pt);
		string getPowerupMessage() { return this->powerup_message; }

		// External forces such as black holes
		void applyForce(btVector3 &force);

		// The type and parameters
		UnitType* getUnitType() { return this->uc; }
		UnitParams* getParams() { return &this->params; }

		void spawnAnimationFinished();

	protected:
		void setState(int new_type);
		void doUse();
		void doLift();
		void doDrop();
		void emptySound();
		void walkSound();
		void enterVehicle(Vehicle *v);
		void leaveVehicle();
		void zoomWeapon();
		void resetIdleTime();
		void dieAnimSound();
		void detectCollision();

		void createKinematicCtlr(btTransform & loc);
		void createDynamicCtlr(btTransform & loc);
		void removeCtlr();

	private:
		/**
		* A pickup applied when the unit is first created
		**/
		static PickupType* initial_pickup;
};
