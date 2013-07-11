#pragma once
#include <SDL.h>
#include "../rage.h"
#include "../gamestate.h"

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}

using namespace std;


/**
* Inits the Lua standard libraries, and nukes some non-safe ones (like IO and OS)
**/
void lua_standard_libs(lua_State *L);


/**
* vector3, from lua_vector3_lib.cpp
**/
void load_vector3_lib(lua_State *L);
double * new_vector3(lua_State *L, float x, float y, float z);
double * get_vector3(lua_State *L, int i);

#define new_vector3bt(L, v) new_vector3(L, (v).getX(), (v).getY(), (v).getZ())


/**
* unitinfo, from lua_unitinfo_lib.cpp
**/
void load_unitinfo_lib(lua_State *L);
UnitQueryResult * new_unitinfo(lua_State *L, UnitQueryResult* src);
UnitQueryResult * get_unitinfo(lua_State *L, int i);


/**
* hudlabel lua class for HUDLabel c++ class
**/
void load_hudlabel_lib(lua_State *L);
void new_hudlabel(lua_State *L, HUDLabel* src);
