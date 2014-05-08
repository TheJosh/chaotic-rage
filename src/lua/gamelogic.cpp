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
* Returns faglse if there is an error
**/
bool GameLogic::execScript(string code)
{
	int res = luaL_dostring(L, code.c_str());
	
	if (res != 0) {
		string msg = "Failed to execute GameType script: ";
		msg.append(lua_tostring(L, -1));
		displayMessageBox(msg);
		this->st->gameOver();
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


LUA_DEFINE_RAISE(gamestart)
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
			printf("  %s", lua_toboolean(L, i) ? "true" : "faglse");
			
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


/**
* Spawn a new NPC unit
*
* TODO: Need a new method for "add at specific location" vs "spawn in based on game rules"
**/
LUA_FUNC(add_npc)
{
	NPC *p;
	
	UnitType *uc = GEng()->mm->getUnitType(*(new string(lua_tostring(L, 1))));
	if (uc == NULL) {
		return luaL_error(L, "Arg #1 is not an available unittype");
	}
	
	AIType *ai = GEng()->mm->getAIType(*(new string(lua_tostring(L, 2))));
	if (uc == NULL) {
		return luaL_error(L, "Arg #2 is not an available aitype");
	}
	
	Faction fac = (Faction) lua_tointeger(L, 3);
	
	
	Zone *zn = gl->map->getSpawnZone(fac);
	if (zn == NULL) {
		return luaL_error(L, "Map does not have any spawnpoints");
	}
	
	p = new NPC(uc, gl->st, zn->getRandomX(), zn->getRandomY(), 2, ai, fac);
	
	for (unsigned int i = 0; i < uc->spawn_weapons.size(); i++) {
		p->pickupWeapon(uc->spawn_weapons.at(i));
	}
	
	gl->st->addUnit(p);
	
	
	return 0;
}


/**
* Spawn a new player unit
*
* @param String Unit type.
* @param int Faction-ID. Use the 'factions' enumeration.
* @param int Player slot, between 1 and N, where N is the number of players in the game, including network.
*
* TODO: Need a new method for "add at specific location" vs "spawn in based on game rules"
**/
LUA_FUNC(add_player)
{
	Player *p;
	
	const char* name = lua_tostring(L, 1);
	UnitType *uc = GEng()->mm->getUnitType(*(new string(name)));
	if (uc == NULL) {
		return luaL_error(L, "Arg #1 is not an available unittype");
	}
	
	Faction fac = (Faction) lua_tointeger(L, 2);
	
	int slot = lua_tointeger(L, 3);
	
	
	Zone *zn = gl->map->getSpawnZone(fac);
	if (zn == NULL) {
		return luaL_error(L, "Map does not have any spawnpoints");
	}
	
	p = new Player(uc, gl->st, zn->getRandomX(), zn->getRandomY(), 0, fac, slot);
	
	// Is it a local player?
	PlayerState *ps = gl->st->localPlayerFromSlot(slot);
	if (ps) {
		ps->p = p;
	}
	
	gl->st->addUnit(p);
	
	return 0;
}


/**
* Kills off the player with a given slot #
*
* TODO: Need a new method for "remove given entity" vs "kill off based on game rules"
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
* Game is over
* First and only paramater is the result - 1 = success, 0 = failure
* Success -> next stage of campaign
**/
LUA_FUNC(game_over)
{
	gl->st->gameOver(lua_tointeger(L, 1));
	return 0;
}


/**
* Drop ammo onto the game level
*
* TODO: This function is a bit specialised - it might be better to just use add_object
**/
LUA_FUNC(ammo_drop)
{
	Pickup *pu;
	
	// special ammocrate for current weapon
	pu = new Pickup(
		GEng()->mm->getPickupType("ammo_current"),
		gl->st,
		gl->st->map->getRandomX(),
		gl->st->map->getRandomY(),
		15
	);
	gl->st->addPickup(pu);
	
	return 0;
}


/**
* Display an alert message
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
* Display an alert message
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


/************************   TODO move this to own file   *****************************/

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
		
		virtual void onMouseMove(Uint16 x, Uint16 y)
		{
			btVector3 hitLocation(0.0f, 0.0f, 0.0f);
			Entity* hitEntity = NULL;
			bool result;

			// Move cursor somewhere else
			btTransform xform1(btQuaternion(0.0f, 0.0f, 0.0f), btVector3(0.0f, 0.0f, 0.0f));
			cursor->setTransform(xform1);

			// Do mouse pick
			result = gl->st->mousePick(x, y, hitLocation, &hitEntity);

			// Move cursor to location
			btTransform xform2(btQuaternion(0.0f, 0.0f, 0.0f), hitLocation);
			cursor->setTransform(xform2);
		}
		
		virtual void onMouseUp(Uint8 button, Uint16 x, Uint16 y)
		{
			btVector3 hitLocation(0.0f, 0.0f, 0.0f);
			Entity* hitEntity = NULL;
			bool result;

			// Do mouse pick
			result = gl->st->mousePick(x, y, hitLocation, &hitEntity);

			// Disable mouse pick
			GEng()->setMouseGrab(true);
			gl->mouse_events = NULL;

			// Return result back to Lua code
			if (result) {
				lua_rawgeti(this->L, LUA_REGISTRYINDEX, this->func);
				new_vector3(L, hitLocation.x(), hitLocation.y(), hitLocation.z());
				lua_pcall(this->L, 1, 0, 0);
			}

			// Cleanup
			cursor->del = true;
			//delete this;  //  error: deleting object of polymorphic class type ‘MousePickHandler’ which has non-virtual destructor might cause undefined behaviour [-Werror=delete-non-virtual-dtor]
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
	PickupType *ot = GEng()->mm->getPickupType("ammo_current");
	Pickup *cursor = new Pickup(ot, gl->st, 0.0f, 0.0f, 0.0f);
	cursor->disableCollision();
	gl->st->addPickup(cursor);

	// Set mouse pick
	GEng()->setMouseGrab(false);
	gl->mouse_events = new MousePickHandler(L, func, cursor);

	return 0;
}

/**************************************************************************************************/


/**
* 
*
* @return String: The currently selected unit type
**/
LUA_FUNC(get_selected_unittype)
{
	lua_pushstring(L, gl->selected_unittype->name.c_str());
	return 1;
}


/**
* Get the view mode
**/
LUA_FUNC(get_viewmode)
{
	lua_pushinteger(L, GEng()->render->viewmode);
	return 1;
}


/**
* Set the view mode
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

	LUA_REG(add_npc);				// TODO: Think up improved
	LUA_REG(add_player);			// entity handling
	LUA_REG(kill_player);			// - this too
	LUA_REG(ammo_drop);				// - and this one

	LUA_REG(show_alert_message);	// TODO: needs network support
	LUA_REG(add_label);				// - this too
	LUA_REG(game_over);				// - and this one

	LUA_REG(get_selected_unittype);	// TODO: other player deets too?

	LUA_REG(get_viewmode);			// TODO: Add a 'camera' object
	LUA_REG(set_viewmode);			// dynamic position, animation, etc.
	
	LUA_REG(mouse_pick);

	
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
	load_hudlabel_lib(L);
	load_vector3_lib(L);
	load_random_lib(L);
	load_dialog_lib(L);
	load_entity_lib(L);
}


