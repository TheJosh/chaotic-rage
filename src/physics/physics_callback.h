// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once

class Entity;


/**
* Custom tick callbacks - function
**/
typedef void (*PhysicsTickCallback)(float delta, Entity *e, void *data1, void *data2);


/**
* Details about all attached functions
**/
class PhysicsCallback
{
	public:
		PhysicsTickCallback func;
		Entity *e;
		void *data1;
		void *data2;
};
