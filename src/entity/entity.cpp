// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "entity.h"
#include <btBulletDynamicsCommon.h>


using namespace std;


/**
* Constructor
**/
Entity::Entity(GameState *st)
{
	this->del = false;
	this->st = st;
	this->body = NULL;
}


/**
* Destructor
**/
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
* Remove the entity from the game world
**/
void Entity::remove()
{
	this->del = true;
}


/**
* Return the world transform for this entity
**/
const btTransform &Entity::getTransform() const
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
* Get the current position from the transformation (by reference)
**/
const btVector3& Entity::getPosition() const
{
	return this->getTransform().getOrigin();
}


/**
* Get the current position from the transformation (by value)
**/
btVector3 Entity::getPositionByVal() const
{
	return this->getTransform().getOrigin();
}


/**
* Set the position
**/
void Entity::setPosition(const btVector3 &p)
{
	btTransform xform = this->getTransform();
	xform.setOrigin(p);
	this->setTransform(xform);
}


/**
* Disable collision for this entity
**/
void Entity::disableCollision()
{
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
}
