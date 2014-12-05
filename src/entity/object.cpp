// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "object.h"
#include <vector>
#include "../game_state.h"
#include "../mod/objecttype.h"
#include "../physics_bullet.h"
#include "../rage.h"
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

	this->body = st->physics->addRigidBody(ot->col_shape, 1.0f, motionState, CG_OBJECT);
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

	this->body = st->physics->addRigidBody(ot->col_shape, 1.0f, motionState, CG_OBJECT);
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
void Object::update(int delta)
{
}


/**
* We have been hit! Take some damage
**/
void Object::takeDamage(int damage)
{
	this->health -= damage;
	if (this->health < 0) this->health = 0;

	for (unsigned int j = 0; j < this->ot->damage_models.size(); j++) {
		ObjectTypeDamage * dam = this->ot->damage_models.at(j);

		if (this->health <= dam->health) {
			st->remAnimPlay(this->anim);
			delete(this->anim);
			this->anim = new AnimPlay(dam->model);
			st->addAnimPlay(this->anim, this);
			break;
		}
	}
}
