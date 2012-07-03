// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include "rage.h"

using namespace std;


ParticleGenerator::ParticleGenerator(ParticleGenType* type, GameState *st, float x, float y, float z) : Entity(st)
{
	this->type = type;
	this->age = 0;
	
	for (int i = 0; i < MAX_SPEWERS; i++) {
		this->spewdelay[i] = 0;
	}
	
	
	// TODO: The colShape should be tied to the wall type.
	btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
	
	// TODO: Store the colshape and nuke at some point
	// collisionShapes.push_back(colShape);
	
	this->body = st->physics->addRigidBody(colShape, 0.1, x, y, z);
	
	this->body->setUserPointer(this);
}

ParticleGenerator::~ParticleGenerator()
{
}


/**
* Creates particles
**/
void ParticleGenerator::update(int delta)
{
	GenSpew *spew;
	
	bool useful = false;
	for (unsigned int i = 0; i < this->type->spewers.size(); i++) {
		if (i > MAX_SPEWERS) break;
		spew = this->type->spewers.at(i);
		
		// If there is a time limit in force, check it hasn't been too long
		if (spew->time != 0 && spew->time < this->age) continue;
		useful = true;
		
		// If there is a delay in force, check enough time has passed
		if (spew->delay) {
			this->spewdelay[i] -= delta;
			if (this->spewdelay[i] > 0) continue;
			this->spewdelay[i] = spew->delay;
		}
		
		btTransform trans;
		this->body->getMotionState()->getWorldTransform(trans);
		
		// TODO: Rethink particle generators for new physics
	}
	
	this->age += delta;
	
	if (! useful) {
		this->hasDied();
	}
}


AnimPlay* ParticleGenerator::getAnimModel()
{
	return NULL;
}

Sound* ParticleGenerator::getSound()
{
	return NULL;
}

