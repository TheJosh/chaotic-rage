// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"

using namespace std;


Object::Object(ObjectType *wt, GameState *st) : Entity(st)
{
	this->wt = wt;
	this->anim = new AnimPlay(wt->model);
	this->health = 10000;
	this->cb = NULL;
}

Object::~Object()
{
	cout << "\n\n\n";
	cout << "Removing the object object!\n";
	cout << "\n\n\n";
	delete (this->anim);
	this->st->delCollideBox(this->cb);
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
	
	if (this->cb == NULL) {
		this->cb = this->st->addCollideBox(0, 0, 30, this, true);
	} else {
		this->st->moveCollideBox(this->cb, (int) this->x, (int) this->y);
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
	
	for (unsigned int j = 0; j < this->wt->damage_models.size(); j++) {
		ObjectTypeDamage * dam = this->wt->damage_models.at(j);
		
		if (this->health <= dam->health) {
			delete(this->anim);
			this->anim = new AnimPlay(dam->model);
			break;
		}
	}
	
	if (this->health == 0) {
		this->st->delCollideBox(this->cb);
		this->cb = NULL;
	}
}

