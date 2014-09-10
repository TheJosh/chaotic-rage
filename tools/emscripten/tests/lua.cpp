#include <iostream>
#include <stdlib.h>

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}

int main(int argc, char** argv)
{
	static lua_State *L;
	
	L = luaL_newstate();
	std::cout << "Loaded" << std::endl;
	lua_close(L);
	
	exit(0);
}

