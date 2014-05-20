// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <stdio.h>
#include <math.h>
#include "../rage.h"
#include "../render_opengl/hud_label.h"
#include "lua_libs.h"

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}

#define MYTYPE "hudlabel"


/**
* Internal function to get a unitinfo from data storage
**/
static HUDLabel * _get(lua_State *L, int i)
{
	if (luaL_checkudata(L, i, MYTYPE) == NULL) {
		luaL_typerror(L, i, MYTYPE);
	}

	HUDLabel ** ptr = (HUDLabel **) lua_touserdata(L,i);
	return *ptr;
}

/**
* Internal function to create a new unitinfo.
**/
static void _new(lua_State *L, HUDLabel* src)
{
	HUDLabel ** ptr = (HUDLabel**) lua_newuserdata(L, sizeof(HUDLabel*));
	*ptr = src;

	luaL_getmetatable(L,MYTYPE);
	lua_setmetatable(L,-2);

	return;
}



#define LUA_FUNC(name) static int name(lua_State *L)

/**
* @metatable unitinfo get
**/
LUA_FUNC(get)
{
	HUDLabel *lab = _get(L,1);
	const char* i = luaL_checkstring(L,2);

	if (strcmp(i, "data") == 0) {
		lua_pushstring(L, (const char*)lab->data.c_str());

	} else if (strcmp(i, "x") == 0) {
		lua_pushnumber(L, lab->x);

	} else if (strcmp(i, "y") == 0) {
		lua_pushnumber(L, lab->y);

	} else if (strcmp(i, "visible") == 0) {
		lua_pushboolean(L, lab->visible);

	} else if (strcmp(i, "r") == 0) {
		lua_pushnumber(L, lab->r);

	} else if (strcmp(i, "g") == 0) {
		lua_pushnumber(L, lab->g);

	} else if (strcmp(i, "b") == 0) {
		lua_pushnumber(L, lab->b);

	} else if (strcmp(i, "a") == 0) {
		lua_pushnumber(L, lab->a);

	} else {
		lua_pushstring(L, "Invalid property for object " MYTYPE);
		lua_error(L);
	}

	return 1;
}

/**
* @metatable unitinfo get
**/
LUA_FUNC(set)
{
	HUDLabel *lab = _get(L,1);
	const char* i = luaL_checkstring(L,2);

	if (strcmp(i, "data") == 0) {
		lab->data.assign(lua_tostring(L, 3));

	} else if (strcmp(i, "x") == 0) {
		lab->x = static_cast<float>(lua_tonumber(L, 3));

	} else if (strcmp(i, "y") == 0) {
		lab->y = static_cast<float>(lua_tonumber(L, 3));

	} else if (strcmp(i, "visible") == 0) {
		lab->visible = (lua_toboolean(L, 3) == 1);

	} else if (strcmp(i, "align") == 0) {
		lab->align = (HUDLabelAlign) lua_tointeger(L, 3);

	} else if (strcmp(i, "r") == 0) {
		lab->r = static_cast<float>(lua_tonumber(L, 3));

	} else if (strcmp(i, "g") == 0) {
		lab->g = static_cast<float>(lua_tonumber(L, 3));

	} else if (strcmp(i, "b") == 0) {
		lab->b = static_cast<float>(lua_tonumber(L, 3));

	} else if (strcmp(i, "a") == 0) {
		lab->a = static_cast<float>(lua_tonumber(L, 3));

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
	HUDLabel *v=_get(L,1);
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
	{ "__newindex",	set },
	{ "__tostring",	tostring },
	{ NULL,			NULL }
};


/**
* Loads the library into a lua state
**/
void load_hudlabel_lib(lua_State *L)
{
	luaL_newmetatable(L,MYTYPE);
	luaL_register(L,NULL,R);
}

/**
* Creates a unitinfo object, returning the lua pointer, and
* putting the object on the stack ready for use
**/
void new_hudlabel(lua_State *L, HUDLabel* src)
{
	_new(L, src);
}
