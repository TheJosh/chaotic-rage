// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"

using namespace std;


Object::Object(ObjectType *ot, GameState *st, float x, float y, float z, float angle) : Entity(st)
{
	this->ot = ot;
	this->anim = new AnimPlay(ot->model);
	this->health = ot->health;

	// TODO: The colShape should be tied to the object type.
	btVector3 sizeHE = ot->model->getBoundingSizeHE();
	btCollisionShape* colShape = new btBoxShape(sizeHE);
	
	// TODO: Store the colshape and nuke at some point
	// collisionShapes.push_back(colShape);
	
	btDefaultMotionState* motionState =
		new btDefaultMotionState(btTransform(
			btQuaternion(btScalar(0), btScalar(0), btScalar(DEG_TO_RAD(angle))),
			st->physics->spawnLocation(x, y, sizeHE.z() * 2.0f)
		));
	
	this->body = st->physics->addRigidBody(colShape, 0.0f, motionState, CG_OBJECT);
	
	this->body->setUserPointer(this);
}

Object::~Object()
{
	delete (this->anim);
	st->physics->delRigidBody(this->body);
}


/**
* Do stuff
**/
void Object::update(int delta)
{
	if (this->anim->isDone()) this->anim->next();
	if (this->health == 0) return;
}

AnimPlay* Object::getAnimModel()
{
	return this->anim;
}

Sound* Object::getSound()
{
	return NULL;
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
			delete(this->anim);
			this->anim = new AnimPlay(dam->model);
			break;
		}
	}
}

