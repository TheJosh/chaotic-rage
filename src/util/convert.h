// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <glm/glm.hpp>
#include <btBulletDynamicsCommon.h>

using namespace std;


/**
* glm::mat4 -> btMatrix3x3
**/
void glmBullet(glm::mat4 in, btMatrix3x3& out);


/**
* glm::mat4 -> btTransform
**/
void glmBullet(glm::mat4 in, btTransform& out);
