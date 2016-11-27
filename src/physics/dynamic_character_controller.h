/*
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


#pragma once
#include <BulletDynamics/Character/btCharacterControllerInterface.h>


class btCollisionShape;
class btConvexShape;
class btRigidBody;
class btCollisionWorld;
class btCollisionDispatcher;
class btPairCachingGhostObject;


ATTRIBUTE_ALIGNED16(class) btDynamicCharacterController : public btCharacterControllerInterface
{
	protected:
		btRigidBody* m_body;
		btVector3 m_walkDirection;
		btScalar m_velocityTimeInterval;
		
	public:
		BT_DECLARE_ALIGNED_ALLOCATOR();

		btDynamicCharacterController(btRigidBody* body);
		~btDynamicCharacterController();

		///btActionInterface interface
		virtual void updateAction(btCollisionWorld* collisionWorld, btScalar deltaTime)
		{
			preStep(collisionWorld);
			playerStep(collisionWorld, deltaTime);
		}

		///btActionInterface interface
		void debugDraw(btIDebugDraw* debugDrawer);

		/// Caller provides a velocity with which the character should move for
		///	the given time period.  After the time period, velocity is reset
		///	to zero.
		/// This call will reset any walk direction set by setWalkDirection().
		/// Negative time intervals will result in no motion.
		virtual void setVelocityForTimeInterval(const btVector3& velocity, btScalar timeInterval);
		
		void reset ();
		void reset (btCollisionWorld* collisionWorld);

		void warp (const btVector3& origin);
		
		void playerStep(btCollisionWorld* collisionWorld, btScalar dt);
};
