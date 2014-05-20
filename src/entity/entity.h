// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <stdint.h>


using namespace std;


enum EntityClass {
	NONE = 0,
	UNIT = 1,
	WALL = 4,
	OBJECT = 5,
	VEHICLE = 6,
	AMMOROUND = 7,
	PICKUP = 8,
};


class GameState;
class AnimPlay;
class Sound;
class btTransform;
class btRigidBody;


class Entity
{
	public:
		virtual EntityClass klass() const { return NONE; };

	public:
		bool del;
		bool visible;
		EID eid;
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
		GameState * getGameState();

		/**
		* Called by entity code to indicate the death of itself
		**/
		void hasDied();

		/**
		* Disable collision for this entity
		**/
		void disableCollision();

		/**
		* Get position. Convinience method for transforms
		**/
		const btVector3& getPosition() const;

		/**
		* Set position. Convinience method for transforms
		**/
		void setPosition(const btVector3 &p);
};
