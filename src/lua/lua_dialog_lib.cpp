// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;


#include <stdio.h>
#include <math.h>
#include "../rage.h"
#include "../gui/textprompt.h"
#include "../game_state.h"
#include "../game_engine.h"

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}


#define LUA_FUNC(name) static int name(lua_State *L)
#define LUA_REG(name) lua_register(L, #name, name)



/**
* Handle prompt events
**/
class PromptTextHandler : public DialogTextPromptHandler {
	private:
		lua_State *L;
		int func;
		
	public:
		PromptTextHandler(lua_State *L, int func)
		{
			this->L = L;
			this->func = func;
		}
		
		virtual ~PromptTextHandler() {}
		
		/**
		* Pass the text back into the lua function
		**/
		virtual void handleText(string text)
		{
			lua_rawgeti(this->L, LUA_REGISTRYINDEX, this->func);
			lua_pushstring(this->L, text.c_str());
			lua_pcall(this->L, 1, 0, 0);
		}
};


/**
* Ask the user a question
**/
LUA_FUNC(prompt_text)
{
	if (! lua_isstring(L, 1)) {
		lua_pushstring(L, "Arg #1 is not a string");
		lua_error(L);
	}
	
	if (! lua_isfunction(L, 2)) {
		lua_pushstring(L, "Arg #1 is not a function");
		lua_error(L);
	}
	
	const char* message = lua_tostring(L, 1);
	
	lua_pushvalue(L, -1);
	int func = luaL_ref(L, LUA_REGISTRYINDEX);
	
	PromptTextHandler* handler = new PromptTextHandler(L, func);
	
	GameState* st = getGameState();
	DialogTextPrompt* dialog = new DialogTextPrompt(st, "AAA", message, handler);
	GEng()->addDialog(dialog);
	
	return 0;
}


/**
* Loads the library into a lua state
**/
void load_dialog_lib(lua_State *L)
{
	LUA_REG(prompt_text);
}

