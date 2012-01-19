/*
* lv3.c
* 3d vectors for Lua 5.0
* Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>
* 03 Dec 2004 11:29:50
* This code is hereby placed in the public domain.
*/

#include <stdio.h>
#include <math.h>

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}

#define MYTYPE		"vector3"


/**
* Internal function to get a vector3 from data storage
**/
static double * _get(lua_State *L, int i)
{
	if (luaL_checkudata(L,i,MYTYPE)==NULL) {
		luaL_typerror(L,i,MYTYPE);
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


/**
* Create a new vector3 and assign the x,y,z attributes
* This is registered as "vector3"
**/
static int Lnew(lua_State *L)
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
* Metatable GET element
**/
static int Lget(lua_State *L)
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
* Metatable SET element
**/
static int Lset(lua_State *L)
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
* Metatable TOSTRING element
**/
static int Ltostring(lua_State *L)
{
	double *v=_get(L,1);
	char s[64];
	sprintf(s,"%s %p",MYTYPE,(void*)v);
	lua_pushstring(L,s);
	return 1;
}

/**
* Metatable ADD element.
* Arguments: Two vectors
**/
static int Ladd(lua_State *L)
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
* Metatable SUB element
* Arguments: Two vectors
**/
static int Lsub(lua_State *L)
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
* Metatable MUL element
* Arguments: A vector and a number
**/
static int Lmul(lua_State *L)
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
* Metatable DIV element
* Arguments: A vector and a number
**/
static int Ldiv(lua_State *L)
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
* Metatable UNM element.
* Arguments: One vector
**/
static int Lunm(lua_State *L)
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
* Metatable LEN element.
* Arguments: One vector
**/
static int Llen(lua_State *L)
{
	double *v=_get(L,1);
	double len = sqrt((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]));
	lua_pushnumber(L, len);
	return 1;
}

/**
* Metatable EQ element.
* Arguments: Two vectors
**/
static int Leq(lua_State *L)
{
	double *v1=_get(L,1);
	double *v2=_get(L,2);
	bool result = ((v1[0] == v2[0]) && (v1[1] == v2[1]) && (v1[2] == v2[2]));
	lua_pushboolean(L, result);
	return 1;
}

/**
* Normalizes a vector
* Arguments: One vector
**/
static int Lnormalize(lua_State *L)
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
* Arguments: Two vectors
**/
static int Lcross(lua_State *L)
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
* Arguments: Two vectors
**/
static int Ldot(lua_State *L)
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
static const luaL_reg R[] =
{
	{ "__index",	Lget		},
	{ "__newindex",	Lset		},
	{ "__tostring",	Ltostring	},
	{ "__add",		Ladd		},
	{ "__sub",		Lsub		},
	{ "__mul",		Lmul		},
	{ "__div",		Ldiv		},
	{ "__unm",		Lunm		},
	{ "__len",		Llen		},
	{ "__eq",		Leq			},
	{ NULL,		NULL		}
};


/**
* Loads the library into a lua state
**/
void load_vector3_lib(lua_State *L)
{
	luaL_newmetatable(L,MYTYPE);
	luaL_register(L,NULL,R);
	
	lua_register(L,"vector3",Lnew);
	lua_register(L,"v3normalize",Lnormalize);
	lua_register(L,"v3normalise",Lnormalize);		// catch both spellings
	lua_register(L,"v3cross",Lcross);
	lua_register(L,"v3dot",Ldot);
}

