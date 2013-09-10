// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include "../rage.h"
#include "../gamestate.h"
#include "../physics_bullet.h"
#include "../render/animplay.h"
#include "../mod/weapontype.h"
#include "ammo_round.h"
#include "unit.h"

using namespace std;


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

	btCollisionShape* colShape = new btBoxShape(btVector3(0.1f, 0.1f, 0.1f));
	btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(xform));
	
	this->body = st->physics->addRigidBody(colShape, 0.0f, motionState, CG_DEBRIS);
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

	btCollisionShape* colShape = new btBoxShape(btVector3(0.1f, 0.1f, 0.1f));
	btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(xform));
	
	this->body = st->physics->addRigidBody(colShape, mass, motionState, CG_DEBRIS);
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
