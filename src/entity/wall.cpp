// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "wall.h"
#include <vector>
#include "../game_state.h"
#include "../mod/walltype.h"
#include "../physics_bullet.h"
#include "../rage.h"
#include "../render_opengl/animplay.h"
#include "entity.h"


using namespace std;


Wall::Wall(WallType *wt, GameState *st, float x, float y, float z, float angle) : Entity(st)
{
	this->wt = wt;
	this->anim = new AnimPlay(wt->model);
	this->health = wt->health;

	st->addAnimPlay(this->anim, this);

	btVector3 size = wt->model->getBoundingSize();

	btDefaultMotionState* motionState =
		new btDefaultMotionState(btTransform(
			btQuaternion(btScalar(0), btScalar(0), btScalar(DEG_TO_RAD(angle))),
			st->physics->spawnLocation(x, y, size.z())));

	this->body = st->physics->addRigidBody(wt->col_shape, 0.0f, motionState, CG_WALL);

	this->body->setUserPointer(this);
}

Wall::~Wall()
{
	st->remAnimPlay(this->anim);
	delete(this->anim);
	st->physics->delRigidBody(this->body);
}


/**
* Do stuff
**/
void Wall::update(int delta)
{
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
			st->remAnimPlay(this->anim);
			delete(this->anim);
			this->anim = new AnimPlay(dam->model);
			st->addAnimPlay(this->anim, this);
			break;
		}
	}
}
