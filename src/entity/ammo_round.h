// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "entity.h"

class AnimPlay;
class AssimpModel;
class GameState;
class Sound;
class Unit;
class WeaponType;
class btCollisionShape;
class btTransform;


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

	private:
		static btCollisionShape* col_shape;

	public:
		AmmoRound(GameState* st, btTransform& xform, WeaponType* wt, AssimpModel* model, Unit* owner, float mass = 0.0f);
		virtual ~AmmoRound();

		virtual void update(int delta);
};
