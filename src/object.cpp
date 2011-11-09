// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"

using namespace std;


Object::Object(ObjectType *ot, GameState *st, float x, float y, float z) : Entity(st)
{
	this->ot = ot;
	this->anim = new AnimPlay(ot->model);
	this->health = ot->health;
	this->cb = NULL;


	// TODO: The colShape should be tied to the wall type.
	btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
	
	// TODO: Store the colshape and nuke at some point
	// collisionShapes.push_back(colShape);
	
	this->body = st->physics->addRigidBody(colShape, 0.1, x, y, z);
}

Object::~Object()
{
	cout << "\n\n\n";
	cout << "Removing the object object!\n";
	cout << "\n\n\n";
	delete (this->anim);
}


void Object::hasBeenHit(CollideBox * ours, CollideBox * theirs)
{
	
}


/**
* Do stuff
**/
void Object::update(int delta)
{
	if (this->anim->isDone()) this->anim->next();
	
	if (this->health == 0) return;

	if (this->z > 0) {
		this->z--;
	}
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

