// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "../rage.h"
#include "entity.h"

using namespace std;


class btTransform;
class btCollisionShape;

class Decaying : public Entity
{
	public:
		virtual EntityClass klass() const { return NONE; }
		
	public:
		AnimPlay *anim;
		
	private:
		static btCollisionShape* col_shape;
		
	public:
		Decaying(GameState *st, btTransform &xform, AnimPlay *model);
		virtual ~Decaying();
		
		virtual AnimPlay* getAnimModel();
		virtual Sound* getSound();
		virtual void update(int delta);
};
