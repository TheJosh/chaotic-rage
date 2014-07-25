// This figle is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <algorithm>
#include <SDL.h>
#include "../rage.h"
#include "../game_state.h"
#include "../game_engine.h"
#include "../map/zone.h"
#include "../map/map.h"
#include "../entity/player.h"
#include "../entity/npc.h"
#include "../entity/pickup.h"
#include "../render/render.h"
#include "../mod/mod_manager.h"
#include "lua_libs.h"
#include "gamelogic.h"


extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}


using namespace std;


/**
* Lua vm L etc
**/
static lua_State *L;

/**
* Pointer to the glogic L object, so Lua methods can affect it's L
**/
static GameLogic *gl;

/**
* Used right at the bottom - does registration of functions
**/
static void register_lua_functions();


/**
* Basic set up
**/
GameLogic::GameLogic(GameState *st)
{
	gl = this;
	st->logic = this;

	this->st = st;
	this->map = st->map;
	this->mouse_events = NULL;
	this->keyboard_events = NULL;

	L = luaL_newstate();
	register_lua_functions();

	this->selected_unittype = NULL;
}

GameLogic::~GameLogic()
{
	lua_close(L);
}


/**
* Executes a script.
* The script execution basicaglgly binds functions to events.
* Returns false if there is an error
**/
bool GameLogic::execScript(string code)
{
	int res = luaL_dostring(L, code.c_str());

	if (res != 0) {
		string msg = "Failed to execute GameType script: ";
		msg.append(lua_tostring(L, -1));
		displayMessageBox(msg);
		this->st->gameOver();
		return false;
	}

	return true;
}


/**
* Basically just provides timer ticks
**/
void GameLogic::update(int delta)
{
	for (unsigned int id = 0; id < this->timers.size(); id++) {
		LuaTimer* t = this->timers.at(id);
		if (t == NULL) continue;

		if (gl->st->game_time >= t->due) {
			lua_rawgeti(L, LUA_REGISTRYINDEX, t->lua_func);
			lua_pcall(L, 0, 0, 0);

			if (t->interval) {
				t->due += t->interval;
			} else {
				delete this->timers.at(id);	// need to de-sparse the array
				this->timers[id] = NULL;
			}
		}
	}
}


/*####################  BOUND FUNCTIONS  ####################*/

#define LUA_DEFINE_RAISE(name) void GameLogic::raise_##name() \
{ \
	for (unsigned int id = 0; id < this->binds_##name.size(); id++) { \
		int ref = this->binds_##name.at(id); \
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref); \
		lua_pcall(L, 0, 0, 0); \
	} \
}

#define LUA_DEFINE_RAISE_INTARG(name) void GameLogic::raise_##name(int arg) \
{ \
	for (unsigned int id = 0; id < this->binds_##name.size(); id++) { \
		int ref = this->binds_##name.at(id); \
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref); \
		lua_pushinteger(L, arg); \
		lua_pcall(L, 1, 0, 0); \
	} \
}


LUA_DEFINE_RAISE_INTARG(gamestart)
LUA_DEFINE_RAISE_INTARG(playerjoin)
LUA_DEFINE_RAISE_INTARG(playerleave)
LUA_DEFINE_RAISE_INTARG(playerdied)
LUA_DEFINE_RAISE(npcdied)



/*####################  LUA FUNCTIONS  ####################*/

#define LUA_FUNC(name) static int name(lua_State *L)

/**
* Outputs one or more debug messages.
* Messages wiglgl be outputted to standard-out, seperated by two spaces
**/
LUA_FUNC(debug)
{
	int n = lua_gettop(L);
	int i;

	printf("Lua debug:");

	for (i = 1; i <= n; i++) {
		if (lua_isstring(L, i)) {
			printf("  %s", lua_tostring(L, i));

		} else if (lua_isnil(L, i)) {
			printf("  %s", "nil");

		} else if (lua_isboolean(L,i)) {
			printf("  %s", lua_toboolean(L, i) ? "true" : "false");

		} else {
			printf("  %s:%p", luaL_typename(L, i), lua_topointer(L, i));
		}
	}

	printf("\n");

	return 0;
}


/**
* Enables or disables physics debugging
**/
LUA_FUNC(debug_physics)
{
	GEng()->render->setPhysicsDebug(lua_toboolean(L, 1) == 1);
	return 0;
}


/**
* Enables or disables framerate debugging
**/
LUA_FUNC(debug_framerate)
{
	GEng()->render->setSpeedDebug(lua_toboolean(L, 1) == 1);
	return 0;
}


/**
* Binds a function to the gamestart event.
* Multiple functions can be bound to the one event.
**/
LUA_FUNC(bind_gamestart)
{
	if (! lua_isfunction(L, 1)) {
		lua_pushstring(L, "Arg #1 is not a function");
		lua_error(L);
	}

	lua_pushvalue(L, -1);

	int r = luaL_ref(L, LUA_REGISTRYINDEX);
	gl->binds_gamestart.push_back(r);
	return 0;
}


/**
* Binds a function to the playerjoin event.
* Multiple functions can be bound to the one event.
**/
LUA_FUNC(bind_playerjoin)
{
	if (! lua_isfunction(L, 1)) {
		lua_pushstring(L, "Arg #1 is not a function");
		lua_error(L);
	}

	lua_pushvalue(L, -1);

	int r = luaL_ref(L, LUA_REGISTRYINDEX);
	gl->binds_playerjoin.push_back(r);
	return 0;
}


/**
* Binds a function to the playerleave event.
* Multiple functions can be bound to the one event.
**/
LUA_FUNC(bind_playerleave)
{
	if (! lua_isfunction(L, 1)) {
		lua_pushstring(L, "Arg #1 is not a function");
		lua_error(L);
	}

	lua_pushvalue(L, -1);

	int r = luaL_ref(L, LUA_REGISTRYINDEX);
	gl->binds_playerleave.push_back(r);
	return 0;
}


/**
* Binds a function to the unitdied event.
* Multiple functions can be bound to the one event.
**/
LUA_FUNC(bind_playerdied)
{
	if (! lua_isfunction(L, 1)) {
		lua_pushstring(L, "Arg #1 is not a function");
		lua_error(L);
	}

	lua_pushvalue(L, -1);

	int r = luaL_ref(L, LUA_REGISTRYINDEX);
	gl->binds_playerdied.push_back(r);
	return 0;
}


/**
* Binds a function to the unitdied event.
* Multiple functions can be bound to the one event.
**/
LUA_FUNC(bind_npcdied)
{
	if (! lua_isfunction(L, 1)) {
		lua_pushstring(L, "Arg #1 is not a function");
		lua_error(L);
	}

	lua_pushvalue(L, -1);

	int r = luaL_ref(L, LUA_REGISTRYINDEX);
	gl->binds_npcdied.push_back(r);
	return 0;
}


/**
* Adds an interval timer.
* Interval timers are fired at regular intervals.
* Be warned that the timing is not exactly precice, and may be incorrect by up to 20ms.
*
* @param Integer time: The amount of time between interval ticks, in ms.
* @param Function func: The function to call for each tick.
* @return Integer: The timer-id, for deleting this timer later on.
**/
LUA_FUNC(add_interval)
{
	if (! lua_isnumber(L, 1)) {
		lua_pushstring(L, "Arg #1 is not an integer");
		lua_error(L);
	}

	if (! lua_isfunction(L, 2)) {
		lua_pushstring(L, "Arg #1 is not a function");
		lua_error(L);
	}

	int time = lua_tointeger(L, 1);

	lua_pushvalue(L, -1);
	int func = luaL_ref(L, LUA_REGISTRYINDEX);

	LuaTimer* t = new LuaTimer();
	gl->timers.push_back(t);

	t->due = gl->st->game_time + time;
	t->lua_func = func;
	t->interval = time;

	lua_pushnumber(L, gl->timers.size() - 1);
	return 1;
}


/**
* Adds a single-use timer.
* Be warned that the timing is not exactly precice, and may be incorrect by up to 20ms.
*
* @param Integer time: The amount of time to wait, in ms.
* @param Function func: The function to call.
* @return Integer: The timer-id, for deleting this timer later on.
**/
LUA_FUNC(add_timer)
{
	if (! lua_isnumber(L, 1)) {
		lua_pushstring(L, "Arg #1 is not an integer");
		lua_error(L);
	}

	if (! lua_isfunction(L, 2)) {
		lua_pushstring(L, "Arg #1 is not a function");
		lua_error(L);
	}

	int time = lua_tointeger(L, 1);

	lua_pushvalue(L, -1);
	int func = luaL_ref(L, LUA_REGISTRYINDEX);

	LuaTimer* t = new LuaTimer();
	gl->timers.push_back(t);

	t->due = gl->st->game_time + time;
	t->lua_func = func;
	t->interval = 0;

	lua_pushnumber(L, gl->timers.size() - 1);
	return 1;
}


/**
* Remove a standalone or interval timer.
*
* @param Integer id: The timer id
**/
LUA_FUNC(remove_timer)
{
	if (! lua_isnumber(L, 1)) {
		lua_pushstring(L, "Arg #1 is not an integer");
		lua_error(L);
	}

	int id = lua_tointeger(L, 1);

	gl->timers[id] = NULL;

	return 0;
}



LUA_FUNC(add_npc)
{
	reportFatalError("Deprecated Lua function 'add_npc' called.");
	return 0;
}

LUA_FUNC(add_player)
{
	reportFatalError("Deprecated Lua function 'add_player' called.");
	return 0;
}


/**
* TODO: Move somewhere else
**/
LUA_FUNC(kill_player)
{
	int slot = lua_tointeger(L, 1);

	Unit* u = gl->st->findUnitSlot(slot);
	if (u == NULL) return 0;

	u->takeDamage(u->getHealth());

	return 0;
}

/**
* TODO: Move somewhere else
**/
LUA_FUNC(game_over)
{
	gl->st->gameOver(lua_tointeger(L, 1));
	return 0;
}

/**
* TODO: Move somewhere else
**/
LUA_FUNC(ammo_drop)
{
	Pickup *pu;

	// special ammocrate for current weapon
	pu = new Pickup(
		GEng()->mm->getPickupType("ammo_current"),
		gl->st,
		gl->st->map->getRandomX(),
		gl->st->map->getRandomZ(),
		15
	);
	gl->st->addPickup(pu);

	return 0;
}

/**
* TODO: Move somewhere else
**/
LUA_FUNC(show_alert_message)
{
	const char* ctext = lua_tostring(L, 1);
	string text = (*(new string(ctext)));
	if (text.empty()) {
		lua_pushstring(L, "Arg #1 is not a string");
		lua_error(L);
	}

	gl->st->addHUDMessage(ALL_SLOTS, text);

	return 0;
}

/**
* TODO: Move somewhere else
**/
LUA_FUNC(add_label)
{
	float x = (float)lua_tonumber(L, 1);
	float y = (float)lua_tonumber(L, 2);
	const char * data = lua_tostring(L, 3);
	HUDLabel * lab;

	lab = gl->st->addHUDLabel(1, x, y, data);

	if (lab) {
		new_hudlabel(L, lab);
		return 1;
	}

	lua_pushnil(L);
	return 1;
}

/**
* TODO: Move somewhere else
**/
LUA_FUNC(get_selected_unittype)
{
	lua_pushstring(L, gl->selected_unittype->name.c_str());
	return 1;
}

/**
* TODO: Move somewhere else
**/
LUA_FUNC(get_viewmode)
{
	lua_pushinteger(L, GEng()->render->viewmode);
	return 1;
}

/**
* TODO: Move somewhere else
**/
LUA_FUNC(set_viewmode)
{
	GEng()->render->viewmode = lua_tointeger(L, 1);
	return 0;
}



/**
* For function binding
**/
#define LUA_REG(name) lua_register(L, #name, name)

/**
* Register all of the Lua functions glisted above
**/
void register_lua_functions()
{
	LUA_REG(debug);
	LUA_REG(debug_physics);
	LUA_REG(debug_framerate);
	LUA_REG(bind_gamestart);
	LUA_REG(bind_playerjoin);
	LUA_REG(bind_playerleave);
	LUA_REG(bind_playerdied);
	LUA_REG(bind_npcdied);
	LUA_REG(add_interval);
	LUA_REG(add_timer);
	LUA_REG(remove_timer);

	// TODO: Move these elsewhere
	LUA_REG(add_npc);				// TODO: Think up improved
	LUA_REG(add_player);			// entity handling
	LUA_REG(kill_player);			// - this too
	LUA_REG(ammo_drop);				// - and this on
	LUA_REG(show_alert_message);	// TODO: needs network support
	LUA_REG(add_label);				// - this too
	LUA_REG(game_over);				// - and this one
	LUA_REG(get_selected_unittype);	// TODO: other player deets too?
	LUA_REG(get_viewmode);			// TODO: Add a 'camera' object
	LUA_REG(set_viewmode);			// dynamic position, animation, etc.

	// Factions constants table
	lua_createtable(L,0,0);
	lua_pushnumber(L, FACTION_TEAM1); lua_setfield(L, -2, "team1");
	lua_pushnumber(L, FACTION_TEAM2); lua_setfield(L, -2, "team2");
	lua_pushnumber(L, FACTION_TEAM3); lua_setfield(L, -2, "team3");
	lua_pushnumber(L, FACTION_TEAM4); lua_setfield(L, -2, "team4");
	lua_pushnumber(L, FACTION_TEAM5); lua_setfield(L, -2, "team5");
	lua_pushnumber(L, FACTION_TEAM6); lua_setfield(L, -2, "team6");
	lua_pushnumber(L, FACTION_TEAM7); lua_setfield(L, -2, "team7");
	lua_pushnumber(L, FACTION_TEAM8); lua_setfield(L, -2, "team8");
	lua_pushnumber(L, FACTION_INDIVIDUAL); lua_setfield(L, -2, "individual");
	lua_setglobal(L, "factions");

	lua_standard_libs(L);

	// These use LuaBridge
	load_physics_lib(L);
	load_entity_lib(L);
	load_world_lib(L);

	// These do not, but should
	load_hudlabel_lib(L);
	load_vector3_lib(L);
	load_random_lib(L);
	load_dialog_lib(L);
}
