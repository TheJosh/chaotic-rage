// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;
//
// Original copyright block:
//   Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>
//   03 Dec 2004 11:29:50
//   This code is hereby placed in the public domain.


#include <stdio.h>
#include <math.h>

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}

#define MYTYPE		"vector3"
#define BUFFER_MAX		64


/**
* Internal function to get a vector3 from data storage
**/
static double * _get(lua_State *L, int i)
{
	if (luaL_checkudata(L,i,MYTYPE)==NULL) {
		const char *msg = lua_pushfstring(L, MYTYPE " expected, got %s", luaL_typename(L, i));
		luaL_argerror(L, i, msg);
	}

	return (double*)lua_touserdata(L,i);
}

/**
* Internal function to create a new vector3
**/
static double * _new(lua_State *L)
{
	double *v=(double*)lua_newuserdata(L,3*sizeof(double));
	luaL_getmetatable(L,MYTYPE);
	lua_setmetatable(L,-2);
	return v;
}



#define LUA_FUNC(name) static int name(lua_State *L)

/**
* Create a new vector3 and assign the x,y,z attributes
*
* @param optional number X
* @param optional number Y
* @param optional number Z
* @return vector3
**/
LUA_FUNC(vector3)
{
	double *v;
	lua_settop(L,3);
	v=_new(L);
	v[0]=luaL_optnumber(L,1,0);
	v[1]=luaL_optnumber(L,2,0);
	v[2]=luaL_optnumber(L,3,0);
	return 1;
}


/**
* @metatable vector3 get
**/
LUA_FUNC(get)
{
	double *v=_get(L,1);
	const char* i=luaL_checkstring(L,2);
	switch (*i) {
		case '1': case 'x': case 'r': lua_pushnumber(L,v[0]); break;
		case '2': case 'y': case 'g': lua_pushnumber(L,v[1]); break;
		case '3': case 'z': case 'b': lua_pushnumber(L,v[2]); break;
		default: lua_pushnil(L); break;
	}
	return 1;
}

/**
* @metatable vector3 set
**/
LUA_FUNC(set)
{
	double *v=_get(L,1);
	const char* i=luaL_checkstring(L,2);
	double t=luaL_checknumber(L,3);
	switch (*i) {
		case '1': case 'x': case 'r': v[0]=t; break;
		case '2': case 'y': case 'g': v[1]=t; break;
		case '3': case 'z': case 'b': v[2]=t; break;
		default: break;
	}
	return 1;
}

/**
* @metatable vector3 tostring
**/
LUA_FUNC(tostring)
{
	double *f=_get(L,1);
	char s[BUFFER_MAX];
	snprintf(s,BUFFER_MAX,"vector3 [%5.3f,%5.3f,%5.3f]",f[0],f[1],f[2]);
	lua_pushstring(L,s);
	return 1;
}

/**
* @metatable vector3 add
* @param vector3 A
* @param vector3 B
**/
LUA_FUNC(add)
{
	double *v1=_get(L,1);
	double *v2=_get(L,2);
	double *vR;
	lua_settop(L,3);
	vR=_new(L);
	vR[0] = v1[0] + v2[0];
	vR[1] = v1[1] + v2[1];
	vR[2] = v1[2] + v2[2];
	return 1;
}

/**
* @metatable vector3 sub
* @param vector3 A
* @param vector3 B
**/
LUA_FUNC(sub)
{
	double *v1=_get(L,1);
	double *v2=_get(L,2);
	double *vR;
	lua_settop(L,3);
	vR=_new(L);
	vR[0] = v1[0] - v2[0];
	vR[1] = v1[1] - v2[1];
	vR[2] = v1[2] - v2[2];
	return 1;
}

/**
* @metatable vector3 mul
* @param vector3 A
* @param number B
**/
LUA_FUNC(mul)
{
	double *v1=_get(L,1);
	double f=lua_tonumber(L,2);
	double *vR;
	lua_settop(L,3);
	vR=_new(L);
	vR[0] = v1[0] * f;
	vR[1] = v1[1] * f;
	vR[2] = v1[2] * f;
	return 1;
}

/**
* @metatable vector3 div
* @param vector3 A
* @param number B
**/
LUA_FUNC(div)
{
	double *v1=_get(L,1);
	double f=lua_tonumber(L,2);
	double *vR;
	lua_settop(L,3);
	vR=_new(L);
	vR[0] = v1[0] / f;
	vR[1] = v1[1] / f;
	vR[2] = v1[2] / f;
	return 1;
}

/**
* @metatable vector3 unm
* @param vector3 A
**/
LUA_FUNC(unm)
{
	double *v1=_get(L,1);
	double *vR;
	lua_settop(L,3);
	vR=_new(L);
	vR[0] = 0 - v1[0];
	vR[1] = 0 - v1[1];
	vR[2] = 0 - v1[2];
	return 1;
}

/**
* @metatable vector3 len
* @param vector3 A
**/
LUA_FUNC(len)
{
	double *v=_get(L,1);
	double len = sqrt((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]));
	lua_pushnumber(L, len);
	return 1;
}

/**
* @metatable vector3 eq
* @param vector3 A
* @param vector3 B
**/
LUA_FUNC(eq)
{
	double *v1=_get(L,1);
	double *v2=_get(L,2);
	bool result = ((v1[0] == v2[0]) && (v1[1] == v2[1]) && (v1[2] == v2[2]));
	lua_pushboolean(L, result);
	return 1;
}

/**
* Normalizes a vector
*
* @param vector3 V
* @return vector3
**/
LUA_FUNC(normalize)
{
	double *v=_get(L,1);
	double len = sqrt((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]));
	double *vR;
	lua_settop(L,3);
	vR=_new(L);
	vR[0] = v[0] / len;
	vR[1] = v[1] / len;
	vR[2] = v[2] / len;
	return 1;
}

/**
* Cross product
*
* @param vector3 A
* @param vector3 B
* @return vector3
**/
LUA_FUNC(cross)
{
	double *v1=_get(L,1);
	double *v2=_get(L,2);
	double *vR;
	lua_settop(L,3);
	vR=_new(L);
	vR[0] = -v1[2] * v2[1] + v1[1] * v2[2];
	vR[1] = v1[2] * v2[0] - v1[0] * v2[2];
	vR[2] = -v1[1] * v2[0] + v1[0] * v2[1];
	return 1;
}

/**
* Dot product
*
* @param vector3 A
* @param vector3 B
* @return vector3
**/
LUA_FUNC(dot)
{
	double *v1=_get(L,1);
	double *v2=_get(L,2);
	double dot = (v1[0] * v2[0]) + (v1[1] * v2[1]) + (v1[2] * v2[2]);
	lua_pushnumber(L, dot);
	return 1;
}


/**
* Metatable definition
**/
static const luaL_Reg R[] =
{
	{ "__index",	get		},
	{ "__newindex",	set		},
	{ "__tostring",	tostring},
	{ "__add",		add		},
	{ "__sub",		sub		},
	{ "__mul",		mul		},
	{ "__div",		div		},
	{ "__unm",		unm		},
	{ "__len",		len		},
	{ "__eq",		eq		},
	{ NULL,		NULL		}
};


/**
* Loads the library into a lua state
**/
void load_vector3_lib(lua_State *L)
{
	luaL_newmetatable(L,MYTYPE);
	#if LUA_VERSION_NUM == 502
		luaL_setfuncs(L, R, 0);
	#elif LUA_VERSION_NUM == 501
		luaL_register(L,NULL,R);
	#endif

	lua_register(L,"vector3",vector3);
	lua_register(L,"v3normalize",normalize);
	lua_register(L,"v3normalise",normalize);		// catch both spellings
	lua_register(L,"v3cross",cross);
	lua_register(L,"v3dot",dot);
}

/**
* Creates a vector3 object, returning the lua pointer, and
* putting the object on the stack ready for use
**/
double * new_vector3(lua_State *L, float x, float y, float z)
{
	double *v;
	lua_settop(L,3);
	v=_new(L);
	v[0]=x;
	v[1]=y;
	v[2]=z;
	return v;
}

double * get_vector3(lua_State *L, int i)
{
	return _get(L, i);
}
