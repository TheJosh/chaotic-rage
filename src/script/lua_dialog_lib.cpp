// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;


#include <stdio.h>
#include <math.h>
#include <SDL.h>
#include "gamelogic.h"
#include "../rage.h"
#include "../gui/textprompt.h"
#include "../gui/listprompt.h"
#include "../gui/textbox.h"
#include "../gui/buttonbar.h"
#include "../mod/gametype.h"
#include "../game_state.h"
#include "../game_engine.h"
#include "../entity/entity.h"
#include "../entity/pickup.h"
#include "../mod/mod_manager.h"

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}

#include "LuaBridge/LuaBridge.h"

// This can be removed when we move to LuaBridge
#include "lua_libs.h"
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
		lua_pushstring(L, "Arg #2 is not a function");
		lua_error(L);
	}

	const char* message = lua_tostring(L, 1);

	lua_pushvalue(L, -1);
	int func = luaL_ref(L, LUA_REGISTRYINDEX);

	PromptTextHandler* handler = new PromptTextHandler(L, func);

	GameState* st = getGameState();
	DialogTextPrompt* dialog = new DialogTextPrompt(st, st->gt->title, message, handler);
	GEng()->addDialog(dialog);

	return 0;
}


/**
* Handler for mouse clicks.
* Set up by the mouse_pick function.
**/
class MousePickHandler : public MouseEventHandler {
	private:
		lua_State *L;
		int func;
		Entity* cursor;

	public:
		MousePickHandler(lua_State *L, int func, Entity* cursor)
		{
			this->L = L;
			this->func = func;
			this->cursor = cursor;
		}

		virtual ~MousePickHandler() {}

		/**
		* On move - update cursor location
		**/
		virtual bool onMouseMove(Uint16 x, Uint16 y)
		{
			btVector3 hitLocation(0.0f, 0.0f, 0.0f);
			Entity* hitEntity = NULL;

			// Move cursor somewhere else
			btTransform xform1(btQuaternion(0.0f, 0.0f, 0.0f), btVector3(0.0f, 0.0f, 0.0f));
			cursor->setTransform(xform1);

			// Do mouse pick
			bool success = getGameState()->mousePick(x, y, hitLocation, &hitEntity);

			// Move cursor to location
			if (success) {
				btTransform xform2(btQuaternion(0.0f, 0.0f, 0.0f), hitLocation);
				cursor->setTransform(xform2);
			}

			return true;
		}

		/**
		* On down - nothing to events don't propagate back to the game
		**/
		virtual bool onMouseDown(Uint8 button, Uint16 x, Uint16 y)
		{
			return true;
		}

		/**
		* On up - call lua code and then destroy
		**/
		virtual bool onMouseUp(Uint8 button, Uint16 x, Uint16 y)
		{
			btVector3 hitLocation(0.0f, 0.0f, 0.0f);
			Entity* hitEntity = NULL;
			bool result;

			// Do mouse pick
			result = getGameState()->mousePick(x, y, hitLocation, &hitEntity);

			// Disable mouse pick
			GEng()->setMouseGrab(true);
			getGameState()->logic->mouse_events = NULL;

			// Return result back to Lua code
			if (result) {
				lua_rawgeti(this->L, LUA_REGISTRYINDEX, this->func);
				new_vector3(this->L, hitLocation.x(), hitLocation.y(), hitLocation.z());
				if (lua_pcall(this->L, 1, 0, 0) != 0) {
					reportFatalError("Lua boo boo: " + std::string(lua_tostring(this->L, -1)));
				}
			}

			// Cleanup
			cursor->del = true;
			delete this;

			return true;
		}
};


/**
* Allow the user to pick an object on screen
**/
LUA_FUNC(mouse_pick)
{
	if (! lua_isfunction(L, 1)) {
		lua_pushstring(L, "Arg #1 is not a function");
		lua_error(L);
	}

	// Grab function pointer
	lua_pushvalue(L, -1);
	int func = luaL_ref(L, LUA_REGISTRYINDEX);

	// TODO: Get a better cursor
	PickupType *ot = GEng()->mm->getPickupType("cursor");
	Pickup *cursor = new Pickup(ot, getGameState(), 0.0f, 0.0f, 0.0f);
	cursor->disableCollision();
	getGameState()->addPickup(cursor);

	// Set mouse pick
	GEng()->setMouseGrab(false);
	getGameState()->logic->mouse_events = new MousePickHandler(L, func, cursor);

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
		explicit LuaButtonBarHandler(luabridge::LuaRef func) : func(func) {}
		virtual ~LuaButtonBarHandler() {}

	public:
		/**
		* This will be called with the index of the button which was pressed
		**/
		virtual void handleButton(int index)
		{
			func(index + 1);		// Lua array indexes start at 1
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
* Receives list prompt selection events
**/
class LuaListPromptHandler : public DialogListPromptHandler {
	private:
		luabridge::LuaRef func;

	public:
		explicit LuaListPromptHandler(luabridge::LuaRef func) : func(func) {}
		virtual ~LuaListPromptHandler() {}

	public:
		/**
		* This will be called with the index of the button which was pressed
		**/
		virtual void handleSelection(int index)
		{
			func(index + 1);		// Lua array indexes start at 1
		}
};

/**
* Add a new ListPrompt dialog
**/
DialogListPrompt* addDialogListPrompt(string title, string message, lua_State* L)
{
	luabridge::LuaRef argLabels = luabridge::LuaRef::fromStack(L, 3);
	if (! argLabels.isTable()) return NULL;

	luabridge::LuaRef argFunc = luabridge::LuaRef::fromStack(L, 4);
	if (! argFunc.isFunction()) return NULL;

	// Load labels into vector
	vector<string>* items = new vector<string>();
	int len = argLabels.length();
	for (int i = 1; i <= len; i++) {
		items->push_back(argLabels[i]);
	}

	// Create dialog
	DialogListPrompt* dialog = new DialogListPrompt(title, message, items, new LuaListPromptHandler(argFunc));
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
	LUA_REG(mouse_pick);


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
		.deriveClass<DialogListPrompt, Dialog>("DialogListPrompt")
		.endClass()
		.addFunction("addDialogTextBox", &addDialogTextBox)
		.addFunction("addDialogButtonBar", &addDialogButtonBar)
		.addFunction("addDialogListPrompt", &addDialogListPrompt)
		.addFunction("basicKeyPress", &basicKeyPress)
	.endNamespace();
}
