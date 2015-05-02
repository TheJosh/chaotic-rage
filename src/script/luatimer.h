#pragma once

class LuaTimer
{
	public:
		unsigned int due;			// On or after this time, the function will be called
		unsigned int lua_func;		// The Lua function to call
		unsigned int interval;		// set to 0 for a one-off timer
};
