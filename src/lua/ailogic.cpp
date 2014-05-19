// This figle is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <algorithm>
#include <SDL.h>

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

#include "../rage.h"
#include "../game_state.h"
#include "../mod/unittype.h"
#include "../entity/npc.h"
#include "../entity/unit.h"
#include "../entity/pickup.h"
#include "../util/btCRKinematicCharacterController.h"
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include "luatimer.h"
#include "lua_libs.h"
#include "ailogic.h"


using namespace std;

class UnitParams;


/**
* Pointer to the currently active Lua vm. Set with AILogic::ActiveLuaState
**/
static lua_State *L;

/**
* Pointer to the currently active AILogic class. Set with AILogic::ActiveLuaState
**/
static AILogic *gl;

/**
* Used right at the bottom - does registration of functions
**/
static void register_lua_functions();



/**
* Basic set up
**/
AILogic::AILogic(Unit *u)
{
	this->u = u;
	this->st = u->getGameState();
	this->lua = luaL_newstate();
	
	this->ActiveLuaState();
	register_lua_functions();
	
	this->dir = btVector3(0,0,0);
	this->dir_flag = true;
	this->speed = 0;
	this->needEndFiring = false;
}

AILogic::~AILogic()
{
	lua_close(this->lua);
}


/**
* This has to be called before any Lua code is executed, to set the static vars correctly
* It makes the AI code not thread safe.
**/
void AILogic::ActiveLuaState()
{
	L = this->lua;
	gl = this;
}


/**
* Executes a script.
* The script execution basicaglgly binds functions to events.
* Returns false if there is an error
**/
bool AILogic::execScript(string code)
{
	this->ActiveLuaState();
	
	int res = luaL_dostring(L, code.c_str());
	
	if (res != 0) {
		string msg = "Failed to execute AI script: ";
		msg.append(lua_tostring(L, -1));
		displayMessageBox(msg);
		this->u->takeDamage(this->u->health);
	}
	
	return true;
}


/**
* Basically just provides timer ticks
**/
void AILogic::update(int delta)
{
	this->ActiveLuaState();
	
	// Fire timers
	for (unsigned int id = 0; id < this->timers.size(); id++) {
		LuaTimer* t = this->timers.at(id);
		if (t == NULL) continue;
		
		if (this->st->game_time >= t->due) {
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
	
	// Handle movement
	if (this->dir_flag) {
		this->dir.setY(0.0f);

		btScalar walkSpeed = u->params.max_speed * 1.0f/60.0f;		// Physics runs at 60hz

		// Rotation update
		btVector3 fwd = btVector3(0.0, 0.0, 1.0);
		btVector3 axis = fwd.cross(this->dir);
		axis.normalize();
		float angle = acos(this->dir.dot(fwd));
		btQuaternion rot = btQuaternion(axis, angle).normalize();
		u->ghost->getWorldTransform().setBasis(btMatrix3x3(rot));

		// Position update
		u->character->setWalkDirection(this->dir * walkSpeed);

		this->dir_flag = false;
	}
	
	// If we should end firing, then end firing
	if (this->needEndFiring) {
		this->u->endFiring();
		this->needEndFiring = false;
	}
}


/*####################  BOUND FUNCTIONS  ####################*/

#define LUA_DEFINE_RAISE(name) void AILogic::raise_##name() \
{ \
	this->ActiveLuaState(); \
	for (unsigned int id = 0; id < this->binds_##name.size(); id++) { \
		int ref = this->binds_##name.at(id); \
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref); \
		lua_pcall(L, 0, 0, 0); \
	} \
}

#define LUA_DEFINE_RAISE_INTARG(name) void AILogic::raise_##name(int arg) \
{ \
	this->ActiveLuaState(); \
	for (unsigned int id = 0; id < this->binds_##name.size(); id++) { \
		int ref = this->binds_##name.at(id); \
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref); \
		lua_pushinteger(L, arg); \
		lua_pcall(L, 1, 0, 0); \
	} \
}


#if(0)
LUA_DEFINE_RAISE(gamestart)
LUA_DEFINE_RAISE_INTARG(playerjoin)
LUA_DEFINE_RAISE(playerdied)
LUA_DEFINE_RAISE(npcdied)
#endif


/*####################  LUA FUNCTIONS  ####################*/

#define LUA_FUNC(name) static int name(lua_State *L)


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
* Returns all visible units for this current unit
**/
LUA_FUNC(visible_units)
{
	list<UnitQueryResult> * uqr = gl->st->findVisibleUnits(gl->u);
	
	lua_newtable(L);
	
	int i = 0;
	for (list<UnitQueryResult>::iterator it = uqr->begin(); it != uqr->end(); ++it) {
		i++;
		//lua_pushnumber(L, (*it).dist);
		new_unitinfo(L, &(*it));
		lua_rawseti (L, 1, i);
	}
	
	delete(uqr);
	
	return 1;
}


/**
* Return a unitinfo object with information about the currently running AI unit
*
* @return unitinfo
**/
LUA_FUNC(get_info)
{
	UnitQueryResult *uqr = new UnitQueryResult();
	uqr->u = gl->u;
	uqr->dist = 0;

	new_unitinfo(L, uqr);

	delete (uqr);

	return 1;
}


/**
* Sets the AI going in a certain direction, at a certain speed.
*
* The vector is normalized before use, and the speed is limited by the unit type configuration.
* If the speed is omitted or zero, the maximum possible speed for the unit type will be used.
*
* @param vector3 direction Direction of travel
* @param optional vector3 speed Speed, in meters per second
**/
LUA_FUNC(move)
{
	double * v = get_vector3(L, 1);
	btVector3 walkDirection = btVector3((float)v[0], (float)v[1], (float)v[2]);
	walkDirection.normalize();

	gl->dir = walkDirection;
	gl->dir_flag = true;

	float speed = (float)lua_tonumber(L, 2);
	if (speed != 0.0f) {
		gl->speed = MAX(speed, gl->u->getParams()->max_speed);
	} else {
		gl->speed = gl->u->getParams()->max_speed;
	}
	
	return 0;
}


/**
* Does a melee attack
*
* @param vector3 direction Direction of attack
**/
LUA_FUNC(melee)
{
	double * v = get_vector3(L, 1);
	btVector3 attackDirection = btVector3((float)v[0], (float)v[1], (float)v[2]);
	attackDirection.normalize();
	
	// Determine direction matrix from direction vector
	btVector3 fwd = btVector3(0.0, 0.0, 1.0);
	btVector3 axis = fwd.cross(attackDirection);
	axis.normalize();
	float angle = acos(attackDirection.dot(fwd));
	btQuaternion rot = btQuaternion(axis, angle).normalize();
	btMatrix3x3 matRot = btMatrix3x3(rot);
	
	gl->u->meleeAttack(matRot);
	
	return 0;
}


/**
* Begin firing the gun
**/
LUA_FUNC(begin_firing)
{
	gl->u->beginFiring();
	return 0;
}


/**
* End firing the gun
**/
LUA_FUNC(end_firing)
{
	gl->u->endFiring();
	return 0;
}


/**
* Sets firing going and sets a flag to end firing the next tick
*
* For a non-continous gun this will fire a single shot
* For a continous gun you might get a few shots
**/
LUA_FUNC(fire)
{
	gl->u->beginFiring();
	gl->needEndFiring = true;
	return 0;
}


/**
* Makes the AI stop movement.
**/
LUA_FUNC(stop)
{
	gl->dir = btVector3(0.0f, 0.0f, 0.0f);
	gl->dir_flag = true;
	gl->speed = 0;
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
	lua_standard_libs(L);
	load_vector3_lib(L);
	load_unitinfo_lib(L);
	load_random_lib(L);
	
	LUA_REG(add_interval);
	LUA_REG(add_timer);
	LUA_REG(remove_timer);
	LUA_REG(visible_units);
	LUA_REG(get_info);
	LUA_REG(move);
	LUA_REG(melee);
	LUA_REG(begin_firing);
	LUA_REG(end_firing);
	LUA_REG(fire);
	LUA_REG(stop);
}


