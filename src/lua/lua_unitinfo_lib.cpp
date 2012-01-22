// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <stdio.h>
#include <math.h>
#include "../rage.h"
#include "lua_libs.h"

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}

#define MYTYPE "unitinfo"


/**
* Internal function to get a unitinfo from data storage
**/
static UnitQueryResult * _get(lua_State *L, int i)
{
	if (luaL_checkudata(L, i, MYTYPE) == NULL) {
		luaL_typerror(L, i, MYTYPE);
	}
	
	return (UnitQueryResult*) lua_touserdata(L,i);
}

/**
* Internal function to create a new unitinfo.
* Clones an existing UnitQueryResult object to avoid scoping issues.
**/
static UnitQueryResult * _new(lua_State *L, UnitQueryResult* src)
{
	UnitQueryResult * ptr = (UnitQueryResult*) lua_newuserdata(L, sizeof(UnitQueryResult));
	memcpy(ptr, src, sizeof(UnitQueryResult));
	luaL_getmetatable(L,MYTYPE);
	lua_setmetatable(L,-2);
	return ptr;
}



#define LUA_FUNC(name) static int name(lua_State *L)

/**
* @metatable unitinfo get
**/
LUA_FUNC(get)
{
	UnitQueryResult *uqr = _get(L,1);
	const char* i = luaL_checkstring(L,2);

	if (strcmp(i, "dist") == 0) {
		lua_pushnumber(L, uqr->dist);

	} else if (strcmp(i, "location") == 0) {
		btTransform trans;
		uqr->u->getRigidBody()->getMotionState()->getWorldTransform(trans);
		btVector3 vecO = trans.getOrigin();
		new_vector3bt(L, vecO);

	} else {
		lua_pushstring(L, "Invalid property for object " MYTYPE);
		lua_error(L);
	}

	return 1;
}

/**
* @metatable unitinfo tostring
**/
LUA_FUNC(tostring)
{
	UnitQueryResult *v=_get(L,1);
	char s[64];
	sprintf(s,"%s %p",MYTYPE,(void*)v);
	lua_pushstring(L,s);
	return 1;
}


/**
* Metatable definition
**/
static const luaL_reg R[] =
{
	{ "__index",	get },
	{ "__tostring",	tostring },
	{ NULL,			NULL }
};


/**
* Loads the library into a lua state
**/
void load_unitinfo_lib(lua_State *L)
{
	luaL_newmetatable(L,MYTYPE);
	luaL_register(L,NULL,R);
}

/**
* Creates a unitinfo object, returning the lua pointer, and
* putting the object on the stack ready for use
**/
UnitQueryResult * new_unitinfo(lua_State *L, UnitQueryResult* src)
{
	return _new(L, src);
}

UnitQueryResult * get_unitinfo(lua_State *L, int i)
{
	return _get(L, i);
}