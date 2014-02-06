// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include "../rage.h"
#include "../game_state.h"
#include "../render_opengl/animplay.h"
#include "../mod/walltype.h"
#include "wall.h"


using namespace std;


Wall::Wall(WallType *wt, GameState *st, float x, float y, float z, float angle) : Entity(st)
{
	this->wt = wt;
	this->anim = new AnimPlay(wt->model);
	this->health = wt->health;
	
	btVector3 sizeHE = wt->model->getBoundingSizeHE();
	
	btDefaultMotionState* motionState =
		new btDefaultMotionState(btTransform(
			btQuaternion(btScalar(0), btScalar(0), btScalar(DEG_TO_RAD(angle))),
			st->physics->spawnLocation(x, y, sizeHE.z() * 2.0f)
		));
	
	this->body = st->physics->addRigidBody(wt->col_shape, 0.0f, motionState, CG_WALL);
	
	this->body->setUserPointer(this);
}

Wall::~Wall()
{
	delete (this->anim);
	st->physics->delRigidBody(this->body);
}


/**
* Do stuff
**/
void Wall::update(int delta)
{
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
void Wall::takeDamage(float damage)
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

