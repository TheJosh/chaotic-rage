// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "object.h"
#include <vector>
#include "../game_state.h"
#include "../mod/objecttype.h"
#include "../physics/physics_bullet.h"
#include "../types.h"
#include "../render_opengl/animplay.h"
#include "entity.h"


using namespace std;


/**
* Create an object at the given map coords
* Will be spawned on the ground at the specified location
**/
Object::Object(ObjectType *ot, GameState *st, float x, float z) : Entity(st)
{
	this->ot = ot;
	this->health = ot->health;

	this->anim = new AnimPlay(ot->model);
	this->anim->setAnimation(0);
	st->addAnimPlay(this->anim, this);

	btVector3 size = ot->model->getBoundingSize();
	btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(
		btQuaternion(0.0f, 0.0f, 0.0f),
		st->physics->spawnLocation(x, z, size.z())
	));

	this->body = st->physics->addRigidBody(ot->col_shape, ot->mass, motionState, CG_OBJECT);
	this->body->setUserPointer(this);
}


/**
* Create an object at the given 3D coords
**/
Object::Object(ObjectType *ot, GameState *st, float x, float y, float z) : Entity(st)
{
	this->ot = ot;
	this->health = ot->health;

	this->anim = new AnimPlay(ot->model);
	this->anim->setAnimation(0);
	st->addAnimPlay(this->anim, this);

	btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(
		btQuaternion(0.0f, 0.0f, 0.0f),
		btVector3(x, y, z)
	));

	this->body = st->physics->addRigidBody(ot->col_shape, ot->mass, motionState, CG_OBJECT);
	this->body->setUserPointer(this);
}

Object::~Object()
{
	st->remAnimPlay(this->anim);
	delete(this->anim);
	st->physics->delRigidBody(this->body);
}


/**
* Do stuff
**/
void Object::update(float delta)
{
}


/**
* We have been hit! Take some damage
**/
void Object::takeDamage(float damage)
{
	this->health -= damage;
	if (this->health <= 0.0f) {
		this->del = true;
	}
}
