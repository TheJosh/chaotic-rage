// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "unit.h"

class AILogic;
class AIType;
class GameState;
class UnitType;


class NPC : public Unit
{
	friend class AILogic;


	private:
		AILogic *logic;

	public:
		NPC(UnitType *uc, GameState *st, Faction fac, AIType *ai, float x, float z);
		NPC(UnitType *uc, GameState *st, Faction fac, AIType *ai, float x, float y, float z);
		NPC(UnitType *uc, GameState *st, Faction fac, AIType *ai, btTransform & loc);
		virtual ~NPC();

	private:
		int vals[4];

	public:
		virtual void update(float delta);
		virtual void die();
		void setWalkVelocity(const btVector3& velocity);

};
