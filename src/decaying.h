// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class Decaying : public Entity
{
	public:
		virtual EntityClass klass() const { return NONE; }
		
	public:
		AssimpModel *model;
		AnimPlay *anim;
		
	public:
		Decaying(GameState *st, btTransform &xform, AssimpModel *model);
		virtual ~Decaying();
		
	public:
		virtual AnimPlay* getAnimModel();
		virtual Sound* getSound();
		virtual void update(int delta);
};
