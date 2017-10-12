// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "npc.h"
#include "../../game_state.h"
#include "../../script/ailogic.h"
#include "../../script/gamelogic.h"
#include "../../mod/aitype.h"
#include "../../types.h"
#include "../../physics/kinematic_character_controller.h"
#include "unit.h"

class UnitType;

using namespace std;


/**
* Spawn npc using map X/Z coords
**/
NPC::NPC(UnitType *uc, GameState *st, Faction fac, AIType *ai, float x, float z) : Unit(uc, st, fac, x, z)
{
	vals[0] = vals[1] = vals[2] = vals[3] = 0;

	this->logic = new AILogic(this);
	this->logic->execScript(ai->script);
}


/**
* Spawn npc using X/Y/Z coords
**/
NPC::NPC(UnitType *uc, GameState *st, Faction fac, AIType *ai, float x, float y, float z) : Unit(uc, st, fac, x, y, z)
{
	vals[0] = vals[1] = vals[2] = vals[3] = 0;

	this->logic = new AILogic(this);
	this->logic->execScript(ai->script);
}


/**
* Spawn npc using specific coordinates
**/
NPC::NPC(UnitType *uc, GameState *st, Faction fac, AIType *ai, btTransform & loc) : Unit(uc, st, fac, loc)
{
	vals[0] = vals[1] = vals[2] = vals[3] = 0;

	this->logic = new AILogic(this);
	this->logic->execScript(ai->script);
}


NPC::~NPC()
{
	delete(this->logic);
	this->logic = NULL;
}


/**
* Try to find a player and attack them
**/
void NPC::update(float delta)
{
	logic->update();

	this->walkSound();
	this->resetIdleTime();

	Unit::update(delta);
}


/**
* Try to find a player and attack them
**/
void NPC::physicsUpdate(float delta)
{
	btVector3 moveDir = logic->getDir();
	moveDir.setY(0.0f);

	// Rotation update
	if (moveDir.length2() > btScalar(0.0001)) {
		btVector3 fwd = btVector3(0.0, 0.0, 1.0);
		btVector3 axis = fwd.cross(moveDir);
		axis.normalize();
		float angle = acos(moveDir.dot(fwd));
		btQuaternion rot = btQuaternion(axis, angle).normalize();
		this->ghost->getWorldTransform().setBasis(btMatrix3x3(rot));
		this->aim_dir = rot;
	}

	btScalar walkSpeed = this->params.max_speed;
	this->setWalkVelocity(moveDir * walkSpeed);

	Unit::physicsUpdate(delta);
}


/**
* The npc has died
**/
void NPC::die()
{
	this->st->logic->raise_npcdied();

	Unit::die();
}


/**
* Set the walk direction and velocity, in meters per second
**/
void NPC::setWalkVelocity(const btVector3& velocity)
{
	this->char_ctlr->setVelocityForTimeInterval(velocity, 1.0f);
}
