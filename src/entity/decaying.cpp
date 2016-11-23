// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "decaying.h"
#include "../game_state.h"
#include "../physics/physics_bullet.h"
#include "../render_opengl/animplay.h"
#include "entity.h"

class GameState;

using namespace std;


btCollisionShape* Decaying::col_shape;


#define MAX_AGE (5 * 60 * 1000)


Decaying::Decaying(GameState *st, const btTransform &xform, AnimPlay *play, float mass) : Entity(st)
{
	this->anim = new AnimPlay(*play);

	if (! Decaying::col_shape) {
		Decaying::col_shape = new btBoxShape(btVector3(0.1f, 0.1f, 0.1f));
	}

	btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(xform));

	this->body = st->physics->addRigidBody(Decaying::col_shape, mass, motionState, CG_DEBRIS);
	st->addAnimPlay(this->anim, this);
	
	this->remove_after = st->game_time + MAX_AGE;
}

Decaying::~Decaying()
{
	st->remAnimPlay(this->anim);
	delete(this->anim);
	st->physics->delRigidBody(this->body);
}


/**
* Do stuff
**/
void Decaying::update(int delta)
{
	if (this->st->game_time > this->remove_after) {
		this->del = true;
	}
}

