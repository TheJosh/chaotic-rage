#pragma once
#include <iostream>
#include <SDL.h>
#include "../rage.h"

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}

using namespace std;


/**
* Vector3, from lua_vector3_lib.cpp
**/
void load_vector3_lib(lua_State *L);


/**
* Unitinfo, from lua_unitinfo_lib.cpp
**/
void load_unitinfo_lib(lua_State *L);
UnitQueryResult * create_unitinfo(lua_State *L, UnitQueryResult* src);
