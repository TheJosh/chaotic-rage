// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include "../util/quadratic.h"

using namespace std;


/**
* Create a chost object for collision detection
**/
btGhostObject* create_ghost(btTransform& xform, float radius);


/**
* If there is anything within the ghost radius, apply damage as appropriate
**/
void apply_ghost_damage(btGhostObject* ghost, Quadratic damage, float radius);


/**
* Check if a ghost has triggered or not
**/
bool check_ghost_triggered(btGhostObject* ghost);


