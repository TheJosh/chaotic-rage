// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <btBulletDynamicsCommon.h>
#include "entity.h"


class AnimPlay;
class Sound;
class ObjectType;
class AssimpModel;
class WeaponType;
class Unit;


/**
* Some weapons need to create entities which exist in the game world
* for a period of time. Examples include rockets and mines
**/
class AmmoRound : public Entity
{
	public:
		virtual EntityClass klass() const { return AMMOROUND; }
		
	public:
		WeaponType *wt;
		AnimPlay *anim;
		void* data;
		Unit* owner;
		float mass;

	public:
		AmmoRound(GameState* st, btTransform& xform, WeaponType* wt, AssimpModel* model, Unit* owner);
		AmmoRound(GameState* st, btTransform& xform, WeaponType* wt, AssimpModel* model, Unit* owner, float mass);
		virtual ~AmmoRound();
		
	public:
		virtual AnimPlay* getAnimModel();
		virtual Sound* getSound();
		virtual void update(int delta);
};

