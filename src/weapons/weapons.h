// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include "../rage.h"

using namespace std;


#define WEAPON_TYPE_RAYCAST 1
#define WEAPON_TYPE_DIGDOWN 2
#define WEAPON_TYPE_FLAMETHROWER 3
#define WEAPON_TYPE_TIMED_MINE 4


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
		virtual void doFire(Unit *unit, btTransform &origin);
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
		virtual void doFire(Unit *unit, btTransform &origin);
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
		virtual void doFire(Unit *unit, btTransform &origin);
};


/**
* Mine which goes off after a set amount of time
**/
class WeaponTimedMine : public WeaponType
{
	public:
		int time;		// ms
		AssimpModel* model;

	public:
		virtual void doFire(Unit *unit, btTransform &origin);
		virtual void entityUpdate(AmmoRound *e, int delta);
};

struct WeaponTimedMineData {
	int time;
	btGhostObject* ghost;
};

