// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;


#include <stdio.h>
#include "../physics_bullet.h"

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}

#include "LuaBridge/LuaBridge.h"


/**
* Exposes basic bullet classes
**/
void load_physics_lib(lua_State *L)
{
	luabridge::getGlobalNamespace(L)
	.beginNamespace("physics")

		.beginClass<btVector3>("Vector")
			.addProperty("x", &btVector3::x, &btVector3::setX)
			.addProperty("y", &btVector3::y, &btVector3::setY)
			.addProperty("z", &btVector3::z, &btVector3::setZ)
			.addFunction("length", &btVector3::length)
			.addFunction("dot", &btVector3::dot)
			.addFunction("cross", &btVector3::cross)
			.addFunction("normalize", &btVector3::normalize)
			.addConstructor<void (*) (float, float, float)>()
		.endClass()

		.beginClass<btQuadWord>("QuadWord")
			.addProperty("x", &btQuadWord::x, &btQuadWord::setX)
			.addProperty("y", &btQuadWord::y, &btQuadWord::setY)
			.addProperty("z", &btQuadWord::z, &btQuadWord::setZ)
			.addProperty("w", &btQuadWord::w, &btQuadWord::setW)
		.endClass()

		.deriveClass<btQuaternion, btQuadWord>("Quaternion")
		.endClass()

	.endNamespace();
}

