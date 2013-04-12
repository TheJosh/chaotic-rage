// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include <btBulletDynamicsCommon.h>
#include <list>
#include "rage.h"

using namespace std;


enum EntityClass {
	NONE = 0,
	UNIT = 1,
	WALL = 4,
	OBJECT = 5,
	VEHICLE = 6,
	AMMOROUND = 7,
};


/**
* Type for EIDs - unique entity identifiers, for network play
**/
typedef Uint16 EID;


class Entity
{
	public:
		virtual EntityClass klass() const { return NONE; };
		
	public:
		bool del;
		bool render;
		EID eid;

	protected:
		btRigidBody* body;
		GameState* st;

	protected:
		Entity(GameState *st);
		
	public:
		virtual ~Entity();
		
	public:
		/**
		* Return an AnimModel to render this entity as
		**/
		virtual AnimPlay* getAnimModel() = 0;
		
		/**
		* Return Sounds for this entity
		* This is going to change some time soon!
		**/
		virtual Sound* getSound() = 0;
		
		/**
		* Return the world transform for this entity
		**/
		virtual btTransform &getTransform() { return body->getWorldTransform(); }

		/**
		* Set the world transform for this entity
		**/
		virtual void setTransform(btTransform &t) { body->setWorldTransform(t); }

		/**
		* Update the entity. Time is provided in milliseconds (ms)
		**/
		virtual void update(int delta) = 0;


	public:
		GameState * getGameState();
		
	protected:
		/**
		* Called by entity code to indicate the death of itself
		**/
		void hasDied();
		
};
