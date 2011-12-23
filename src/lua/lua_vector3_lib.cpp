/*
* lv3.c
* 3d vectors for Lua 5.0
* Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>
* 03 Dec 2004 11:29:50
* This code is hereby placed in the public domain.
*/

#include <stdio.h>

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}

#define MYTYPE		"vector3"


static double *Pget(lua_State *L, int i)
{
 if (luaL_checkudata(L,i,MYTYPE)==NULL) luaL_typerror(L,i,MYTYPE);
 return (double*)lua_touserdata(L,i);
}

static double *Pnew(lua_State *L)
{
 double *v=(double*)lua_newuserdata(L,3*sizeof(double));
 luaL_getmetatable(L,MYTYPE);
 lua_setmetatable(L,-2);
 return v;
}

static int Lnew(lua_State *L)			/** vector3([x,y,z]) */
{
 double *v;
 lua_settop(L,3);
 v=Pnew(L);
 v[0]=luaL_optnumber(L,1,0);
 v[1]=luaL_optnumber(L,2,0);
 v[2]=luaL_optnumber(L,3,0);
 return 1;
}

static int Lget(lua_State *L)
{
 double *v=Pget(L,1);
 const char* i=luaL_checkstring(L,2);
 switch (*i) {		/* lazy! */
  case '1': case 'x': case 'r': lua_pushnumber(L,v[0]); break;
  case '2': case 'y': case 'g': lua_pushnumber(L,v[1]); break;
  case '3': case 'z': case 'b': lua_pushnumber(L,v[2]); break;
  default: lua_pushnil(L); break;
 }
 return 1;
}

static int Lset(lua_State *L) {
 double *v=Pget(L,1);
 const char* i=luaL_checkstring(L,2);
 double t=luaL_checknumber(L,3);
 switch (*i) {		/* lazy! */
  case '1': case 'x': case 'r': v[0]=t; break;
  case '2': case 'y': case 'g': v[1]=t; break;
  case '3': case 'z': case 'b': v[2]=t; break;
  default: break;
 }
 return 1;
}

static int Ltostring(lua_State *L)
{
 double *v=Pget(L,1);
 char s[64];
 sprintf(s,"%s %p",MYTYPE,(void*)v);
 lua_pushstring(L,s);
 return 1;
}

static const luaL_reg R[] =
{
	{ "__index",	Lget		},
	{ "__newindex",	Lset		},
	{ "__tostring",	Ltostring	},
	{ NULL,		NULL		}
};

void load_vector3_lib(lua_State *L)
{
 luaL_newmetatable(L,MYTYPE);
 luaL_register(L,NULL,R);
 lua_register(L,"vector3",Lnew);
}
