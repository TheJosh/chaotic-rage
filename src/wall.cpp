// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"

using namespace std;


Wall::Wall(WallType *wt, GameState *st, float x, float y, float z, float angle) : Entity(st)
{
	this->wt = wt;
	this->anim = new AnimPlay(wt->model);
	this->health = wt->health;
	
	
	// TODO: The colShape should be tied to the wall type.
	btCollisionShape* colShape = new btBoxShape(wt->model->getBoundingSizeHE());
	
	// TODO: Store the colshape and nuke at some point
	// collisionShapes.push_back(colShape);
	
	btDefaultMotionState* motionState =
		new btDefaultMotionState(btTransform(
			btQuaternion(btScalar(0), btScalar(0), btScalar(DEG_TO_RAD(angle))),
			btVector3(x,y,z)
		));
	
	this->body = st->physics->addRigidBody(colShape, 0.0, motionState);
	
	this->body->setUserPointer(this);
}

Wall::~Wall()
{
	delete (this->anim);
	st->physics->delRigidBody(this->body);
}


void Wall::hasBeenHit(Entity * that)
{
}


/**
* Do stuff
**/
void Wall::update(int delta)
{
	if (this->anim->isDone()) this->anim->next();
	
	if (this->health == 0) return;
}

AnimPlay* Wall::getAnimModel()
{
	return this->anim;
}

Sound* Wall::getSound()
{
	return NULL;
}


/**
* We have been hit! Take some damage
**/
void Wall::takeDamage(int damage)
{
	this->health -= damage;
	if (this->health < 0) this->health = 0;
	
	for (unsigned int j = 0; j < this->wt->damage_models.size(); j++) {
		WallTypeDamage * dam = this->wt->damage_models.at(j);
		
		if (this->health <= dam->health) {
			delete(this->anim);
			this->anim = new AnimPlay(dam->model);
			break;
		}
	}
}

