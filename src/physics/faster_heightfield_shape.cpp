/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2009 Erwin Coumans  http://bulletphysics.org

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include <LinearMath/btTransformUtil.h>

#include "faster_heightfield_shape.h"


/**
 * Constructor just calls the base constructor
 */
btFasterHeightfieldTerrainShape::btFasterHeightfieldTerrainShape
(
	int heightStickWidth, int heightStickLength, const void* heightfieldData,
	btScalar heightScale, btScalar minHeight, btScalar maxHeight,int upAxis,
	PHY_ScalarType hdt, bool flipQuadEdges
) :
	btHeightfieldTerrainShape(
		heightStickWidth, heightStickLength,
		(void*) heightfieldData, heightScale,
		minHeight, maxHeight,
		upAxis, hdt,
		flipQuadEdges
	)
{}



/**
 * This returns the "raw" (user's initial) height, not the actual height.
 * The actual height needs to be adjusted to be relative to the center
 * of the heightfield's AABB.
 */
btScalar btFasterHeightfieldTerrainShape::getRawHeightFieldValue(int x, int y) const
{
	return m_heightfieldDataFloat[(y * m_heightStickWidth) + x];
}
