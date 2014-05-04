// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;


#include <stdio.h>
#include <math.h>
#include "../rage.h"

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}


#define LUA_FUNC(name) static int name(lua_State *L)
#define LUA_REG(name) lua_register(L, #name, name)


/**
* Return a random integer between min and max
**/
LUA_FUNC(random)
{
	int min = lua_tointeger(L, 1);
	int max = lua_tointeger(L, 2);
	
	lua_pushinteger(L, getRandom(min,max));
	
	return 1;
}


/**
* Return a random floating-point number between min and max
**/
LUA_FUNC(random_float)
{
	float min = static_cast<float>(lua_tonumber(L, 1));
	float max = static_cast<float>(lua_tonumber(L, 2));
	
	lua_pushnumber(L, getRandomf(min,max));
	
	return 1;
}


/**
* Randomly choose one of the arguments specified
**/
LUA_FUNC(random_arg)
{
	int num = lua_gettop(L);
	if (num == 0) return 0;
	
	lua_pushvalue(L, getRandom(1, num));
	
	return 1;
}


/**
* Loads the library into a lua state
**/
void load_random_lib(lua_State *L)
{
	LUA_REG(random);
	LUA_REG(random_float);
	LUA_REG(random_arg);
}

