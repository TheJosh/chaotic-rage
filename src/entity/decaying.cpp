// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include "../rage.h"

using namespace std;


Decaying::Decaying(GameState *st, btTransform &xform, AssimpModel *model) : Entity(st)
{
	this->model = model;
	this->anim = new AnimPlay(model);

	btCollisionShape* colShape = new btBoxShape(btVector3(0.1f, 0.1f, 0.1f));
	btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(xform));
	
	this->body = st->physics->addRigidBody(colShape, 0.0f, motionState, CG_DEBRIS);
}

Decaying::~Decaying()
{
	delete(this->anim);
	st->physics->delRigidBody(this->body);
}


/**
* Do stuff
**/
void Decaying::update(int delta)
{
}

AnimPlay* Decaying::getAnimModel()
{
	return this->anim;
}

Sound* Decaying::getSound()
{
	return NULL;
}
