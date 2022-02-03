// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;


#include <stdio.h>
#include <math.h>
#include "lua_libs.h"
#include "../game_engine.h"
#include "../mod/mod_manager.h"
#include "../mod/weapontype.h"


extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}

#include "LuaBridge/LuaBridge.h"


static luabridge::LuaRef getAllWeaponTypes(lua_State* L)
{
    luabridge::LuaRef out(L, luabridge::newTable(L));

	vector<WeaponType*>* in = GEng()->mm->getAllWeaponTypes();

	for (vector<WeaponType*>::iterator it = in->begin(); it != in->end(); ++it) {
		WeaponType* wt = *it;
		out[wt->name] = wt->title;
	}

    return out;
}


/**
* Information about loaded mods
**/
void load_mod_lib(lua_State *L)
{
	luabridge::getGlobalNamespace(L)
	.beginNamespace("mod")

		.addFunction("getAllWeaponTypes", &getAllWeaponTypes)

	.endNamespace();
}
