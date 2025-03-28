/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2008 Erwin Coumans http://bulletphysics.com

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
#include "LinearMath/btVector3.h"
#include "BulletDynamics/Dynamics/btActionInterface.h"

class btCollisionShape;
class btRigidBody;
class btCollisionWorld;

class btCharacterControllerInterface : public btActionInterface
{
    public:
        btCharacterControllerInterface () {};
        virtual ~btCharacterControllerInterface () {};

        virtual void setVelocityForTimeInterval(const btVector3& velocity, btScalar timeInterval) = 0;
        virtual void reset () = 0;
        virtual void warp (const btVector3& origin) = 0;

        virtual void updateAction(btCollisionWorld* collisionWorld, btScalar deltaTime) = 0;

        virtual bool canJump () const = 0;
        virtual void jump () = 0;
};
