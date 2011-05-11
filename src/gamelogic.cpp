// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <algorithm>
#include <SDL.h>
#include "rage.h"

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}


using namespace std;


/**
* Lua vm state etc
**/
static lua_State *state;

/**
* Pointer to the logic state object, so Lua methods can affect it's state
**/
static GameLogic *l;

/**
* Used right at the bottom - does registration of functions
**/
void register_lua_functions();


/**
* Basic set up
**/
GameLogic::GameLogic(GameState *st)
{
	l = this;
	st->logic = this;
	this->st = st;

	this->mod = st->getMod(0);
	this->map = st->curr_map;
	
	state = lua_open();
	register_lua_functions();
}

GameLogic::~GameLogic()
{
	lua_close(state);
}


/**
* Executes a script.
* The script execution basically binds functions to events.
* Returns false if there is an error
**/
bool GameLogic::execScript(string code)
{
	int res = luaL_dostring(state, code.c_str());
	
	if (res != 0) {
		cerr << lua_tostring(state, -1) << "\n";
		return false;
	}
	
	return true;
}


/**
* Handles all game events and determines what to actaully do to make this into an actual game
**/
void GameLogic::handleEvent(Event * ev)
{
	if (ev->type == GAME_STARTED) {
		this->player_spawn = -1;
		
		this->spawn_timer = 0;
		this->num_zomb = 0;
		this->num_killed = 0;
		this->num_wanted_zomb = 10;
		
	} else if (ev->type == UNIT_DIED) {
		this->num_killed++;
		cout << "Num killed: " << this->num_killed << "\n";

		if (this->num_killed == this->num_wanted_zomb) this->num_wanted_zomb += 10;
	}
}


void GameLogic::update(int delta)
{
	spawn_timer += delta;
	if (this->num_zomb < this->num_wanted_zomb && this->spawn_timer > 250) {
		this->spawnNPC(mod->getUnitType(2), FACTION_TEAM2);
		this->num_zomb++;
		this->spawn_timer -= 250;
	}
	
	if (st->curr_player == NULL && this->player_spawn == -1) {
		// Player has died, show spawn menu
		st->hud->showSpawnMenu();
		this->player_spawn = st->game_time;
		
	} else if (st->curr_player == NULL && st->game_time - this->player_spawn > 1000) {
		// Spawn time over, create player
		st->hud->hideSpawnMenu();
		st->curr_player = this->spawnPlayer(mod->getUnitType(0), FACTION_TEAM1);
		this->player_spawn = -1;
		
	}
	
	if (this->num_zomb == this->num_killed) {
		cout << "All zombies are dead\n";
	}
}


/**
* Spawns a player into the specified faction
**/
Player * GameLogic::spawnPlayer(UnitType *uc, Faction fac)
{
	Player *p;
	
	p = new Player(uc, st);
	st->addUnit(p);
	
	p->pickupWeapon(mod->getWeaponType(0));
	p->pickupWeapon(mod->getWeaponType(1));
	p->pickupWeapon(mod->getWeaponType(2));
	
	Zone *z = map->getSpawnZone(fac);
	if (z == NULL) {
		cerr << "Map does not have any spawnpoints\n";
		exit(1);
	}
	
	p->x = z->getRandomX();
	p->y = z->getRandomY();
	p->fac = fac;
	
	return p;
}

/**
* Spawns a player into the specified faction
**/
NPC * GameLogic::spawnNPC(UnitType *uc, Faction fac)
{
	NPC *p;
	
	p = new NPC(uc, st);
	st->addUnit(p);
	
	p->pickupWeapon(mod->getWeaponType(1));
	
	Zone *z = map->getSpawnZone(fac);
	if (z == NULL) {
		cerr << "Map does not have any spawnpoints\n";
		exit(1);
	}
	
	p->x = z->getRandomX();
	p->y = z->getRandomY();
	p->fac = fac;
	
	return p;
}



/**
* Raises a gamestart event
**/
void GameLogic::raiseGamestart()
{
	for (unsigned int ref = 0; ref <= this->binds_start.size(); ref++) {
		lua_rawgeti(state, LUA_REGISTRYINDEX, ref);
		lua_pcall(state, 0, 0, 0);
	}
}


/*####################  LUA FUNCTIONS  ####################*/

/**
* Outputs one or more debug messages.
* Messages will be outputted to standard-out, seperated by two spaces
**/
static int debug(lua_State *L)
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


static int bind_gamestart(lua_State *L)
{
	if (! lua_isfunction(L, 1)) {
		lua_pushstring(L, "Arg #1 is not a function");
        lua_error(L);
	}

	lua_pushvalue(state, -1);

	int r = luaL_ref(state, LUA_REGISTRYINDEX);
	l->binds_start.push_back(r);
	return 0;
}



/**
* For function binding
**/
#define LUA_FUNC(name) lua_register(state, #name, name)

/**
* Register all of the Lua functions listed above
**/
void register_lua_functions()
{
	LUA_FUNC(debug);

	LUA_FUNC(bind_gamestart);
}


