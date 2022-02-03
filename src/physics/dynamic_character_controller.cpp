// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
// Modified from version in Bullet Physics; original license is below.
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;


/**
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2008 Erwin Coumans  http://bulletphysics.com

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include <stdio.h>

#include "dynamic_character_controller.h"
#include "physics_bullet.h"
#include "../game_state.h"


/**
* btCRKinematicCharacterController is an object that supports a sliding motion in a world.
* It uses a ghost object and convex sweep test to test for upcoming collisions. This is combined with discrete collision detection to recover from penetrations.
* Interaction between btCRKinematicCharacterController and dynamic rigid bodies needs to be explicity implemented by the user.
**/
btDynamicCharacterController::btDynamicCharacterController(btRigidBody* body)
{
	m_body = body;
}


/**
* Destructor
**/
btDynamicCharacterController::~btDynamicCharacterController()
{
}


/**
* Set the velocity
**/
void btDynamicCharacterController::setVelocityForTimeInterval(const btVector3& velocity, btScalar timeInterval)
{
	// Need to add velocity to counter friction, gravity, etc
	m_walkDirection = velocity * 1.5f;
	m_velocityTimeInterval = timeInterval;
}


/**
* Reset the kinematic object
**/
void btDynamicCharacterController::reset()
{
	this->reset(getGameState()->physics->getWorld());
}


/**
* Reset the kinematic object
**/
void btDynamicCharacterController::reset(btCollisionWorld* collisionWorld)
{
	m_walkDirection.setValue(0,0,0);
	m_velocityTimeInterval = 0.0;
}


/**
* Teleport to a given location
**/
void btDynamicCharacterController::warp(const btVector3& origin)
{
	btTransform xform;
	xform.setIdentity();
	xform.setOrigin(origin);
	m_body->setWorldTransform(xform);
}


/**
* The main movement logic
**/
void btDynamicCharacterController::updateAction(btCollisionWorld* collisionWorld, btScalar dt)
{
	btVector3 curVel = m_body->getLinearVelocity();
	btVector3 velChange = m_walkDirection - curVel;

	if (!velChange.fuzzyZero()) {
		m_body->activate(true);
	}
	
	m_body->applyCentralImpulse(velChange);
}


/**
* Special fun for debug drawing - does nothing
**/
void btDynamicCharacterController::debugDraw(btIDebugDraw* debugDrawer)
{
}


bool btDynamicCharacterController::canJump () const
{
	return false;
}


void btDynamicCharacterController::jump ()
{
}
