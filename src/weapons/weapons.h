// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include "../util/quadratic.h"
#include "../rage.h"
#include "../mod/weapontype.h"


using namespace std;


#define WEAPON_TYPE_RAYCAST 1
#define WEAPON_TYPE_UNUSED 2
#define WEAPON_TYPE_FLAMETHROWER 3
#define WEAPON_TYPE_TIMED_MINE 4
#define WEAPON_TYPE_PROXI_MINE 5
#define WEAPON_TYPE_REMOTE_MINE 6
#define WEAPON_TYPE_REMOTE_TRIG 7
#define WEAPON_TYPE_ROCKET 8
#define WEAPON_TYPE_ATTRACTOR 9


extern cfg_opt_t weapconf_raycast [];
extern cfg_opt_t weapconf_mine [];
extern cfg_opt_t weapconf_rocket [];
extern cfg_opt_t weapconf_attractor [];


class PhysicsCallback;


/**
* Weapons which fire by doing a raycast
* This is most common weapons (e.g. machineguns, pistols, etc)
**/
class WeaponRaycast : public WeaponType
{
	public:
		int angle_range;
		float range;
		float damage;
		unsigned int burst;
		float speed;

	public:
		virtual void doFire(Unit *unit, btTransform &origin, float damage_multiplier);
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
		float damage;
		unsigned int burst;

	public:
		virtual void doFire(Unit *unit, btTransform &origin, float damage_multiplier);
};


/**
* Mine which goes off after a set amount of time
**/
class WeaponTimedMine : public WeaponType
{
	public:
		WeaponTimedMine() {}

	public:
		float range;
		float damage;
		int time;		// ms

	public:
		virtual void doFire(Unit *unit, btTransform &origin, float damage_multiplier);
		virtual void entityUpdate(AmmoRound *e, int delta);
};

struct WeaponTimedMineData {
	int time;
	btGhostObject* ghost;
	float damage;
};


/**
* Mine which goes off when a unit is nearby
**/
class WeaponProxiMine : public WeaponType
{
	public:
		WeaponProxiMine() {}

	public:
		float range;
		float damage;

	public:
		virtual void doFire(Unit *unit, btTransform &origin, float damage_multiplier);
		virtual void entityUpdate(AmmoRound *e, int delta);
};

struct WeaponProxiMineData {
	int delay;
	btGhostObject* ghost;
	float damage;
};


/**
* Mine which goes off using a remote trigger unit
**/
class WeaponRemoteMine : public WeaponType
{
	public:
		WeaponRemoteMine() {}

	public:
		float range;
		float damage;

	public:
		virtual void doFire(Unit *unit, btTransform &origin, float damage_multiplier);
		virtual void entityUpdate(AmmoRound *e, int delta);
		void trigger(AmmoRound *e);
};

struct WeaponRemoteMineData {
	btGhostObject* ghost;
	float damage;
};



/**
* Trigger all remote mines
**/
class WeaponRemoteTrigger : public WeaponType
{
	public:
		virtual void doFire(Unit *unit, btTransform &origin, float damage_multiplier);
};


/**
* Mine which goes off when a unit is nearby
**/
class WeaponRocket : public WeaponType
{
	public:
		WeaponRocket() : ammo_model(NULL) {}

	public:
		AssimpModel* ammo_model;
		float range;
		float damage;

	public:
		virtual void doFire(Unit *unit, btTransform &origin, float damage_multiplier);
		virtual void entityUpdate(AmmoRound *e, int delta);
};

struct WeaponRocketData {
	int state;
	PhysicsCallback *cbk;
	btGhostObject* ghost;
	float damage;
};


/**
* Sucks stuff into it's vortex - or pushes stuff out!
**/
class WeaponAttractor : public WeaponType
{
	public:
		WeaponAttractor() : ammo_model(NULL) {}

	public:
		float range;
		int time;
		AssimpModel* ammo_model;
		Quadratic force;
		bool inwards;

	public:
		virtual void doFire(Unit *unit, btTransform &origin, float damage_multiplier);
		virtual void entityUpdate(AmmoRound *e, int delta);
};

struct WeaponAttractorData {
	unsigned int destroy_time;
	btGhostObject* ghost;
};
