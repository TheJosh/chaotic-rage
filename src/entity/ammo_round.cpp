// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "ammo_round.h"
#include "../game_state.h"
#include "../mod/weapontype.h"
#include "../physics/physics_bullet.h"
#include "../render_opengl/animplay.h"
#include "entity.h"

class btCollisionShape;
class btTransform;

using namespace std;


btCollisionShape* AmmoRound::col_shape;


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

	if (!AmmoRound::col_shape && mass != 0.0f) {
		AmmoRound::col_shape = new btBoxShape(btVector3(0.1f, 0.1f, 0.1f));
	} else if (!AmmoRound::col_shape) {
		AmmoRound::col_shape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
	}

	btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(xform));

	this->body = st->physics->addRigidBody(AmmoRound::col_shape, mass, motionState, CG_DEBRIS);
	st->addAnimPlay(this->anim, this);
}


/**
* Remove the ammo round
**/
AmmoRound::~AmmoRound()
{
	st->remAnimPlay(this->anim);
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
