// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


enum EntityClass {
	UNIT = 1,
	PARTICLE = 2,
	PGENERATOR = 3,
	WALL = 4,
};


// Defines the length of a sprite-list
// as returned by the getSprite method of entities
#define SPRITE_LIST_LEN 4


class Entity : public EventListener
{
	public:
		virtual EntityClass klass() const = 0;
		
	public:
		bool del;
		int x;
		int y;
		int angle;
		
	protected:
		GameState* st;
		
	protected:
		Entity(GameState *st);
	
	public:
		~Entity();
		
	public:
		/**
		* DEPRECATED: AnimModel is the new way...
		*
		* Return a SpritePtr to render this entity as
		**/
		virtual void getSprite(SpritePtr list [SPRITE_LIST_LEN]) = 0;
		
		/**
		* Return an AnimModel to render this entity as
		**/
		virtual void getAnimModel(AnimPlay * list [SPRITE_LIST_LEN]) = 0;
		
		/**
		* Update the entity. Time is provided in milliseconds (ms)
		**/
		virtual void update(int delta) = 0;
		
		/**
		* Gets the entities actual width, in pixels
		**/
		virtual int getWidth() { return 1; };
		
		/**
		* Gets the entities actual height, in pixels
		**/
		virtual int getHeight() { return 1; };
		
		
	public:
		GameState * getGameState();
		
};
