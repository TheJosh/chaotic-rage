// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <btBulletDynamicsCommon.h>
#include "../rage.h"

using namespace std;


/**
* Convert GLM to Bullet (matrix)
**/
void glmBullet(glm::mat4 in, btMatrix3x3& out)
{
	out.setFromOpenGLSubMatrix(glm::value_ptr(in));
}


/**
* Convert GLM to Bullet (transform)
**/
void glmBullet(glm::mat4 in, btTransform& out)
{
	out.setFromOpenGLMatrix(glm::value_ptr(in));
}
