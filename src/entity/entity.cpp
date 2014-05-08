// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <btBulletDynamicsCommon.h>
#include "../rage.h"
#include "../game_state.h"
#include "entity.h"


using namespace std;


Entity::Entity(GameState *st)
{
	this->del = false;
	this->visible = true;
	this->st = st;
	this->body = NULL;
}

Entity::~Entity()
{
}


/**
* Return the game state point - it is typically the same pointer everywhere.
**/
GameState * Entity::getGameState()
{
	return this->st;
}


/**
* Has this entity died?
**/
void Entity::hasDied()
{
	this->del = true;
}


/**
* Return the world transform for this entity
**/
btTransform &Entity::getTransform()
{
	return body->getWorldTransform();
}


/**
* Set the world transform for this entity
**/
void Entity::setTransform(btTransform &t)
{
	body->setWorldTransform(t);
}


/**
* Disable collision for this entity
**/
void Entity::disableCollision()
{
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
}

