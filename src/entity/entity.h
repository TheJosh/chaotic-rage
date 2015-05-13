// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once

#include "../rage.h"

class AnimPlay;
class GameState;
class btRigidBody;
class btTransform;
class btVector3;


using namespace std;


/**
* These roughly correspond with the various sub-classes of the Entity class
**/
enum EntityClass {
	NONE = 0,
	UNIT = 1,
	WALL = 4,
	OBJECT = 5,
	VEHICLE = 6,
	AMMOROUND = 7,
	PICKUP = 8,
};


/**
* The main entity class
**/
class Entity
{
	public:
		/**
		* Return the class of entity
		**/
		virtual EntityClass klass() const { return NONE; }

	public:
		bool del;
		EID eid;
		btRigidBody* body;

	protected:
		GameState* st;

	protected:
		explicit Entity(GameState *st);

	public:
		virtual ~Entity();

	public:
		/**
		* Return the world transform for this entity
		**/
		virtual const btTransform &getTransform() const;

		/**
		* Set the world transform for this entity
		**/
		virtual void setTransform(btTransform &t);

		/**
		* Update the entity. Time is provided in milliseconds (ms)
		**/
		virtual void update(int delta) = 0;

	public:
		/**
		* Return the game state for this entity
		**/
		GameState* getGameState();

		/**
		* Remove the entity from the game world
		**/
		void remove();

		/**
		* Disable collision for this entity
		**/
		void disableCollision();

		/**
		* Get position. Convinience method for transforms
		**/
		const btVector3& getPosition() const;
		btVector3 getPositionByVal() const;
		
		/**
		* Set position. Convinience method for transforms
		**/
		void setPosition(const btVector3 &p);
};

