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
	PARTICLE = 2,
	PGENERATOR = 3,
	WALL = 4,
	OBJECT = 5,
};


// Defines the length of a sprite-list
// as returned by the getSprite method of entities
#define SPRITE_LIST_LEN 4


class Entity
{
	public:
		virtual EntityClass klass() const { return NONE; };
		
	public:
		bool del;
		float x;
		float y;
		int z;
		float speed;
		int angle;
		
		btRigidBody* body;
		
	protected:
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
		* Update the entity. Time is provided in milliseconds (ms)
		**/
		virtual void update(int delta) = 0;
		
		/**
		* Called by the collission code when a collission has happened
		**/
		virtual void hasBeenHit(CollideBox * ours, CollideBox * theirs) {};
		
		/**
		* Called by the collission code when a collission didn't happen - but is close
		**/
		virtual void entityClose(Entity * e, float dist) {};


	public:
		GameState * getGameState();
		
	protected:
		/**
		* Called by entity code to indicate the death of itself
		**/
		void hasDied();
		
		/**
		* Return the rigid body for this entity
		**/
		btRigidBody* getRigidBody();
		
};
