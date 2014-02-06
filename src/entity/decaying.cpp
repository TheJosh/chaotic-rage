// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include "../rage.h"
#include "../game_state.h"
#include "../physics_bullet.h"
#include "../render_opengl/animplay.h"
#include "decaying.h"

using namespace std;


btCollisionShape* Decaying::col_shape;


Decaying::Decaying(GameState *st, btTransform &xform, AnimPlay *play) : Entity(st)
{
	this->anim = new AnimPlay(*play);

	if (! Decaying::col_shape) {
		Decaying::col_shape = new btBoxShape(btVector3(0.1f, 0.1f, 0.1f));
	}
	
	btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(xform));
	
	this->body = st->physics->addRigidBody(Decaying::col_shape, 0.0f, motionState, CG_DEBRIS);
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
