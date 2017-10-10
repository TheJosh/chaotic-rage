// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "pickup.h"
#include "../game_state.h"
#include "../mod/pickuptype.h"
#include "../physics/physics_bullet.h"
#include "../render_opengl/animplay.h"
#include "entity.h"


using namespace std;


/**
* Create a pickup at the given map coords
* Will be spawned on the ground at the specified location
**/
Pickup::Pickup(PickupType *pt, GameState *st, float x, float z) : Entity(st)
{
	float scale = 2.5f;

	this->pt = pt;

	this->anim = new AnimPlay(pt->model);
	this->anim->setAnimation(0);
	this->anim->setCustomTransformScale(glm::vec3(scale, scale, scale));
	st->addAnimPlay(this->anim, this);

	btVector3 size = pt->model->getBoundingSize();
	btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(
		btQuaternion(0.0f, 0.0f, 0.0f),
		st->physics->spawnLocation(x, z, size.z())
	));

	this->body = st->physics->addRigidBody(pt->col_shape, 0.0f, motionState, CG_PICKUP);
	this->body->setUserPointer(this);

	this->respawn = false;
	this->inactive_until = 0;
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

	this->respawn = false;
	this->inactive_until = 0;
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
void Pickup::update(float delta)
{
	if (this->inactive_until != 0 && st->game_time > this->inactive_until) {
		this->show();
		this->inactive_until = 0;
	}
}


/**
* Hide the pickup
**/
void Pickup::hide()
{
	st->remAnimPlay(this->anim);
	this->anim = NULL;
	this->body->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
}


/**
* Show the pickup
**/
void Pickup::show()
{
	this->anim = new AnimPlay(pt->model);
	this->anim->setAnimation(0);
	st->addAnimPlay(this->anim, this);
	this->body->setCollisionFlags(0);
}


/**
* Handle interaction by the user
*
* @return True on success, false on failure (e.g. wrong weapon for ammo box)
**/
bool Pickup::doUse(Unit *u)
{
	if (this->inactive_until != 0) return false;
	if (this->pt->doUse(u)) {
		if (this->respawn) {
			this->hide();
			this->inactive_until = st->game_time + 60 * 1000;		// 60 seconds
		} else {
			this->del = true;
		}
		return true;
	} else {
		return false;
	}
}
