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


Pickup::Pickup(PickupType *pt, GameState *st, float x, float y, float z) : Entity(st)
{
	this->pt = pt;

	this->anim = new AnimPlay(pt->model);
	this->anim->setAnimation(0);

	btVector3 sizeHE = pt->model->getBoundingSizeHE();

	btDefaultMotionState* motionState =
		new btDefaultMotionState(btTransform(
			btQuaternion(0.0f, 0.0f, 0.0f),
			st->physics->spawnLocation(x, y, sizeHE.z() * 2.0f)));

	this->body = st->physics->addRigidBody(pt->col_shape, 0.0f, motionState, CG_PICKUP);

	this->body->setUserPointer(this);
}


/**
* Kill this pickup off
**/
Pickup::~Pickup()
{
	delete (this->anim);
	st->physics->delRigidBody(this->body);
}


/**
* Do stuff
**/
void Pickup::update(int delta)
{
}


/**
* Return the 3D model for the pickup
**/
AnimPlay* Pickup::getAnimModel()
{
	return this->anim;
}

/**
* Return a sound for the pickup
**/
Sound* Pickup::getSound()
{
	return NULL;
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
