// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once

#include "rage.h"
#include "game_settings.h"


using namespace std;


/* The maximum number of local players */
#define MAX_LOCAL 2

/* The "slot" for representing all players. */
#define ALL_SLOTS 0


class btVector3;
class Entity;
class Unit;
class Wall;
class Vehicle;
class Pickup;
class AmmoRound;
class GameSettings;

namespace gcn {
	class Gui;
	class SDLInput;
	class Container;
}

/**
* Information about a unit. Exposed to lua as Unitinfo
* This class must remain a POD or you will break the lua binding
**/
class UnitQueryResult
{
	public:
		Unit * u;
		float dist;
};


/**
* Info about the players in the game.
**/
class PlayerState
{
	public:
		GameState *st;
		Player * p;				// Player entity
		HUD* hud;				// Heads-up display rendering (data tables, messages, etc)
		unsigned int slot;		// Slots are numbered from 1 upwards. Used for network play.

	public:
		explicit PlayerState(GameState *st);
		~PlayerState();

	private:
		PlayerState(const PlayerState& disabledCopyConstuctor);
};


/**
* TODO: Remove me
**/
class DebugLine
{
	public:
		btVector3 *a;
		btVector3 *b;
};

#define FPS_SAMPLES 100


/**
* Contains all of the state variables for the game
*
* How to decide if it belongs here or in the GameEngine class:
*  - If it should be network-replicated, it belongs here
*  - If it is local to this instance, it belongs over there
**/
class GameState
{
	friend class RenderOpenGL;
	friend class RenderNull;
	friend class RenderDebug;
	friend class RenderAscii;
	friend class AudioSDLMixer;
	friend class NetServer;
	friend void gameLoop(GameState *st, Render *render, Audio* audio, NetClient* client);

	protected:
		list<Entity*> entities;
		list<Entity*> entities_add;

		vector<Unit*> units;		// leaks: items are not removed
		vector<Wall*> walls;		// leaks: items are not removed

		bool running;
		unsigned int entropy;		// TODO: gamestate -> localplayers

		list<DebugLine*>lines;

		EID eid_next;

	public:
		Map* map;
		GameType* gt;
		GameSettings* gs;

		PlayerState * local_players[MAX_LOCAL];
		unsigned int num_local;
		int last_game_result;

		unsigned int anim_frame;
		unsigned int game_time;

		GameLogic* logic;
		PhysicsBullet* physics;

		SPK::System* particle_system;
		SPK::Renderer* particle_renderer;

	public:
		GameState();
		~GameState();

	public:
		// Adding
		void addUnit(Unit* unit);
		void addWall(Wall* wall);
		void addVehicle(Vehicle* vehicle);
		void addObject(Object* object);
		void addPickup(Pickup* pickup);
		void addAmmoRound(AmmoRound* e);
		void addParticleGroup(SPK::Group* group);

		// Debris
		Entity* deadButNotBuried(Entity* e);
		void scatterDebris(Entity* e, unsigned int num, float force, vector<AssimpModel*>* debris_models);

		// Network bits (EID = entity-id; slots are for players)
		EID getNextEID();
		Entity * getEntity(EID eid);

		// Data queries
		list<UnitQueryResult> * findVisibleUnits(Unit* origin);
		Unit * findUnitSlot(unsigned int slot);
		PlayerState * localPlayerFromSlot(unsigned int slot);
		list<AmmoRound*>* findAmmoRoundsUnit(Unit* u);

		// Start and run
		void preGame();
		void update(int delta);
		void postGame();

		/**
		* The main game loop
		**/
		void gameLoop(Render* render, Audio* audio, NetClient* client);

		// Called by Lua, etc
		void gameOver();
		void gameOver(int result);
		int getLastGameResult();

		// Weapon fun
		vector<WeaponType*>* getSpawnWeapons(UnitType* ut, Faction fac);

		// Entropy
		unsigned int getEntropy(unsigned int slot);
		void increaseEntropy(unsigned int slot);

		// HUD
		void addHUDMessage(unsigned int slot, string text);
		void addHUDMessage(unsigned int slot, string text, string text2);
		HUDLabel* addHUDLabel(unsigned int slot, float x, float y, string data);
		bool mousePick(unsigned int x, unsigned int y, btVector3& hitLocation, Entity** hitEntity);

		// Debugging
		void addDebugLine(btVector3 * a, btVector3 * b);
		void addDebugPoint(float x, float y, float z);
		void addDebugPoint(float x, float y, float z, float len);
};


GameState * getGameState();

