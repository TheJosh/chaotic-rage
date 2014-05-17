// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;


#include <stdio.h>
#include <math.h>
#include <SDL.h>
#include "gamelogic.h"
#include "../rage.h"
#include "../gui/textprompt.h"
#include "../gui/textbox.h"
#include "../gui/buttonbar.h"
#include "../game_state.h"
#include "../game_engine.h"

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}

#include "LuaBridge/LuaBridge.h"


#define LUA_FUNC(name) static int name(lua_State *L)
#define LUA_REG(name) lua_register(L, #name, name)


/**
* TODO: Port everything not on LuaBridge
**/



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
* Add a new TextBox dialog
**/
DialogTextBox* addDialogTextBox(string title)
{
	GameState* st = getGameState();
	DialogTextBox* dialog = new DialogTextBox(st, title);
	GEng()->addDialog(dialog);

	return dialog;
}


/**
* Receives button bar click events
**/
class LuaButtonBarHandler : public DialogButtonBarHandler {
	private:
		luabridge::LuaRef func;

	public:
		LuaButtonBarHandler(luabridge::LuaRef func) : func(func) {}
		virtual ~LuaButtonBarHandler() {}

	public:
		/**
		* This will be called with the index of the button which was pressed
		**/
		virtual void handleButton(int index)
		{
			func(index);
		}
};

/**
* Add a new ButtonBar dialog
**/
DialogButtonBar* addDialogButtonBar(string title, lua_State* L)
{
	luabridge::LuaRef argLabels = luabridge::LuaRef::fromStack(L, 2);
	if (! argLabels.isTable()) return NULL;
	
	luabridge::LuaRef argFunc = luabridge::LuaRef::fromStack(L, 3);
	if (! argFunc.isFunction()) return NULL;
	
	// Load labels into vector
	vector<string> labels;
	int len = argLabels.length();
	for (int i = 1; i <= len; i++) {
		labels.push_back(argLabels[i]);
	}
	
	// Create dialog
	DialogButtonBar* dialog = new DialogButtonBar(title, labels, new LuaButtonBarHandler(argFunc));
	GEng()->addDialog(dialog);
	
	return dialog;
}


/**
* Basic key handler which can only handle one key binding
* Calls a Lua function on key up
**/
class BasicKeyHandler : public KeyboardEventHandler {
	private:
		Uint16 keycode;
		luabridge::LuaRef func;

	public:
		BasicKeyHandler(Uint16 keycode, luabridge::LuaRef func)
			:keycode(keycode), func(func)
			{}

		virtual ~BasicKeyHandler() {}

		/**
		* Handle key press
		**/
		virtual bool onKeyUp(Uint16 keycode)
		{
			if (keycode == this->keycode) {
				func();
				return true;
			} else {
				return false;
			}
		}
};

/**
* Basic (and leaky) key binding system
**/
void basicKeyPress(const char* key, lua_State* L)
{
	luabridge::LuaRef argFunc = luabridge::LuaRef::fromStack(L, 2);
	if (! argFunc.isFunction()) return;
	
	SDL_Keycode keycode = SDL_GetKeyFromName(key);

	getGameState()->logic->keyboard_events = new BasicKeyHandler(keycode, argFunc);
}


/**
* Loads the library into a lua state
**/
void load_dialog_lib(lua_State *L)
{
	LUA_REG(prompt_text);


	luabridge::getGlobalNamespace(L)
	.beginNamespace("ui")
		.beginClass<Dialog>("Dialog")
			.addFunction("close", &Dialog::close)
		.endClass()
		.deriveClass<DialogTextBox, Dialog>("DialogTextBox")
			.addProperty("text", &DialogTextBox::getText, &DialogTextBox::setText)
			.addFunction("append", &DialogTextBox::appendLine)
		.endClass()
		.deriveClass<DialogButtonBar, Dialog>("DialogButtonBar")
		.endClass()
		.addFunction("addDialogTextBox", &addDialogTextBox)
		.addFunction("addDialogButtonBar", &addDialogButtonBar)
		.addFunction("basicKeyPress", &basicKeyPress)
	.endNamespace();
}

