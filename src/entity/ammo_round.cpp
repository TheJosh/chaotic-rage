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

	// Boxes too small may fall through terrain; enforce min size on X and Z axis
	btVector3 box = model->getBoundingSizeHE();
	if (box.x() < 0.4f) box.setX(0.4f);
	if (box.z() < 0.4f) box.setZ(0.4f);
	this->col_shape = new btBoxShape(box);

	btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(xform));

	this->body = st->physics->addRigidBody(this->col_shape, mass, motionState, CG_DEBRIS);
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
	delete(this->col_shape);
}


/**
* Do stuff
**/
void AmmoRound::update(float delta)
{
	if (this->del) return;
	this->wt->entityUpdate(this, delta);
}
