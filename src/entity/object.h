// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "entity.h"

class AnimPlay;
class GameState;
class Sound;
class ObjectType;


class Object : public Entity
{
	public:
		virtual EntityClass klass() const { return OBJECT; }

	public:
		ObjectType* ot;
		int health;

	protected:
		AnimPlay * anim;

	public:
		Object(ObjectType *pt, GameState *st, float x, float z);
		Object(ObjectType *pt, GameState *st, float x, float y, float z);
		virtual ~Object();

	public:
		virtual void update(int delta);

	public:
		void takeDamage(int damage);
};
