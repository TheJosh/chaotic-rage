// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"

using namespace std;


AmmoRound::AmmoRound(GameState* st, btTransform& xform, WeaponType* wt, AssimpModel* model, Unit* owner) : Entity(st)
{
	this->wt = wt;
	this->data = NULL;
	this->anim = new AnimPlay(model);
	this->owner = owner;

	btCollisionShape* colShape = new btBoxShape(btVector3(0.1f, 0.1f, 0.1f));
	btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(xform));
	
	this->body = st->physics->addRigidBody(colShape, 0.0f, motionState, CG_DEBRIS);
}

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
