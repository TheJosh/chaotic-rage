// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "pickup.h"
#include "../game_state.h"
#include "../mod/pickuptype.h"
#include "../physics_bullet.h"
#include "../render_opengl/animplay.h"
#include "entity.h"


using namespace std;


/**
* Create a pickup at the given map coords
* Will be spawned on the ground at the specified location
**/
Pickup::Pickup(PickupType *pt, GameState *st, float x, float z) : Entity(st)
{
	this->pt = pt;

	this->anim = new AnimPlay(pt->model);
	this->anim->setAnimation(0);
	st->addAnimPlay(this->anim, this);

	btVector3 size = pt->model->getBoundingSize();

	btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(
		btQuaternion(0.0f, 0.0f, 0.0f),
		st->physics->spawnLocation(x, z, size.z())
	));

	this->body = st->physics->addRigidBody(pt->col_shape, 0.0f, motionState, CG_PICKUP);
	this->body->setUserPointer(this);
}


/**
* Create a pickup at the given 3D coords
**/
Pickup::Pickup(PickupType *pt, GameState *st, float x, float y, float z) : Entity(st)
{
	this->pt = pt;

	this->anim = new AnimPlay(pt->model);
	this->anim->setAnimation(0);
	st->addAnimPlay(this->anim, this);

	btDefaultMotionState* motionState = new btDefaultMotionState(
		btTransform(btQuaternion(0.0f, 0.0f, 0.0f), btVector3(x, y, z))
	);

	this->body = st->physics->addRigidBody(pt->col_shape, 0.0f, motionState, CG_PICKUP);
	this->body->setUserPointer(this);
}


/**
* Kill this pickup off
**/
Pickup::~Pickup()
{
	st->remAnimPlay(this->anim);
	delete(this->anim);
	st->physics->delRigidBody(this->body);
}


/**
* Do stuff
**/
void Pickup::update(int delta)
{
}


/**
* Handle interaction by the user
*
* @return True on success, false on failure (e.g. wrong weapon for ammo box)
**/
bool Pickup::doUse(Unit *u)
{
	if (this->pt->doUse(u)) {
		this->del = true;
		return true;
	} else {
		return false;
	}
}
