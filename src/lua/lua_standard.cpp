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
	lua_pushcfunction(L, luaopen_base);
	lua_pushstring(L, "");
	lua_call(L, 1, 0);

	lua_pushcfunction(L, luaopen_table);
	lua_pushstring(L, LUA_TABLIBNAME);
	lua_call(L, 1, 0);

	lua_pushcfunction(L, luaopen_string);
	lua_pushstring(L, LUA_STRLIBNAME);
	lua_call(L, 1, 0);

	lua_pushcfunction(L, luaopen_math);
	lua_pushstring(L, LUA_MATHLIBNAME);
	lua_call(L, 1, 0);

	lua_pushnil(L);
	lua_setglobal(L, "dofile");

	lua_pushnil(L);
	lua_setglobal(L, "loadfile");

	lua_pushnil(L);
	lua_setglobal(L, "collectgarbage"); 
}
