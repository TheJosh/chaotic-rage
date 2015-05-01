// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;


#include <stdio.h>

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}


/**
* Loads the library into a lua state
**/
void lua_standard_libs(lua_State *L)
{
	luaL_requiref(L, "", luaopen_base, 1);
	luaL_requiref(L, LUA_TABLIBNAME, luaopen_table, 1);
	luaL_requiref(L, LUA_STRLIBNAME, luaopen_string, 1);
	luaL_requiref(L, LUA_MATHLIBNAME, luaopen_math, 1);
	
	lua_pushnil(L);
	lua_setglobal(L, "dofile");

	lua_pushnil(L);
	lua_setglobal(L, "loadfile");

	lua_pushnil(L);
	lua_setglobal(L, "load");

	lua_pushnil(L);
	lua_setglobal(L, "collectgarbage");
}
