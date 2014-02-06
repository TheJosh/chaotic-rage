// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include "../rage.h"
#include "../game_state.h"
#include "../physics_bullet.h"
#include "../render_opengl/animplay.h"
#include "../mod/weapontype.h"
#include "ammo_round.h"
#include "unit.h"

using namespace std;


btCollisionShape* AmmoRound::col_shape;


/**
* Create an ammo round (static)
**/
AmmoRound::AmmoRound(GameState* st, btTransform& xform, WeaponType* wt, AssimpModel* model, Unit* owner) : Entity(st)
{
	this->wt = wt;
	this->data = NULL;
	this->anim = new AnimPlay(model);
	this->owner = owner;
	this->mass = 0.0f;

	if (! AmmoRound::col_shape) {
		AmmoRound::col_shape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
	}
	
	btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(xform));
	
	this->body = st->physics->addRigidBody(AmmoRound::col_shape, 0.0f, motionState, CG_DEBRIS);
}


/**
* Create an ammo round (dynamic with specified mass)
**/
AmmoRound::AmmoRound(GameState* st, btTransform& xform, WeaponType* wt, AssimpModel* model, Unit* owner, float mass) : Entity(st)
{
	this->wt = wt;
	this->data = NULL;
	this->anim = new AnimPlay(model);
	this->owner = owner;
	this->mass = mass;

	if (! AmmoRound::col_shape) {
		AmmoRound::col_shape = new btBoxShape(btVector3(0.1f, 0.1f, 0.1f));
	}
	
	btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(xform));
	
	this->body = st->physics->addRigidBody(AmmoRound::col_shape, mass, motionState, CG_DEBRIS);
}


/**
* Remove the ammo round
**/
AmmoRound::~AmmoRound()
{
	delete(this->anim);
	st->physics->delRigidBody(this->body);
}


/**
* Do stuff
**/
void AmmoRound::update(int delta)
{
	if (this->del) return;
	this->wt->entityUpdate(this, delta);
}

AnimPlay* AmmoRound::getAnimModel()
{
	return this->anim;
}

Sound* AmmoRound::getSound()
{
	return NULL;
}
