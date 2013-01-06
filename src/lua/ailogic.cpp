// This figle is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <algorithm>
#include <SDL.h>
#include "../rage.h"
#include "luatimer.h"
#include "lua_libs.h"

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}


using namespace std;


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
	this->lua = lua_open();
	
	this->ActiveLuaState();
	register_lua_functions();
	
	this->dir = btVector3(0,0,0);
	this->speed = 0;
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
* Returns faglse if there is an error
**/
bool AILogic::execScript(string code)
{
	this->ActiveLuaState();
	
	int res = luaL_dostring(L, code.c_str());
	
	if (res != 0) {
		string msg = "Lua fault: ";
		msg.append(lua_tostring(L, -1));
		reportFatalError(msg);
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
	
	
	// NEW (kinematic controller)
	this->dir.setY(0.0f);

	btVector3 walkDirection = btVector3(0.0, 0.0, 0.0);
	btTransform xform = u->ghost->getWorldTransform();
	btScalar walkSpeed = u->getSettings()->max_speed * 1.0f/60.0f;		// Physics runs at 60hz

	// Rotation update
	btVector3 fwd = btVector3(0.0, 0.0, 1.0);
	btVector3 axis = fwd.cross(this->dir);
	axis.normalize();
	float angle = acos(this->dir.dot(fwd));
	btQuaternion rot = btQuaternion(axis, angle).normalize();
	xform.setBasis(btMatrix3x3(rot));

	// Position update
	u->character->setWalkDirection(this->dir * walkSpeed);

	// OLD
	/*
	this->dir.setY(0.0f);
	
	// Get transform
	btTransform xform = this->u->getTransform();
	
	this->u->putOnGround(&xform);

	// Rotation update
	btVector3 fwd = btVector3(0.0, 0.0, 1.0);
	btVector3 axis = fwd.cross(this->dir);
	axis.normalize();
	float angle = acos(this->dir.dot(fwd));
	xform.setRotation(btQuaternion(axis, angle).normalize());
	
	// Position update
	btVector3 linearVelocity = this->u->body->getLinearVelocity();
	btScalar currspeed = linearVelocity.length();
	if (currspeed < this->speed) {
		btVector3 move = this->dir * (currspeed + btScalar(this->u->getSettings()->accel));
		this->u->body->setLinearVelocity(move);
	}
	
	// Set transform
	this->u->body->getMotionState()->setWorldTransform(xform);
	this->u->body->setCenterOfMassTransform(xform);
	*/
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
	cout << "raise_" #name "(" << arg << ")\n"; \
	cout << "num binds: " << this->binds_##name.size() << "\n"; \
	for (unsigned int id = 0; id < this->binds_##name.size(); id++) { \
		cout << "bind: " << id << "\n"; \
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
	for (list<UnitQueryResult>::iterator it = uqr->begin(); it != uqr->end(); it++) {
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
	btVector3 walkDirection = btVector3(v[0], v[1], v[2]);
	walkDirection.normalize();
	gl->dir = walkDirection;
	
	float speed = lua_tonumber(L, 2);
	if (speed != 0.0f) {
		gl->speed = MAX(speed, gl->u->getSettings()->max_speed);
	} else {
		gl->speed = gl->u->getSettings()->max_speed;
	}
	
	return 0;
}


/**
* Does a melee attack
**/
LUA_FUNC(melee)
{
	gl->u->meleeAttack();
	
	return 0;
}


/**
* Makes the AI stop movement.
**/
LUA_FUNC(stop)
{
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
	
	LUA_REG(add_interval);
	LUA_REG(add_timer);
	LUA_REG(remove_timer);
	LUA_REG(visible_units);
	LUA_REG(get_info);
	LUA_REG(move);
	LUA_REG(melee);
	LUA_REG(stop);
}


