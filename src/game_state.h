// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once

#include "types.h"
#include <vector>
#include <list>
#include <string>

using namespace std;


/* The maximum number of local players */
#define MAX_LOCAL 2

/* The "slot" for representing all players. */
#define ALL_SLOTS 0


class AmmoRound;
class AnimPlay;
class AssimpModel;
class Audio;
class Entity;
class GameLogic;
class GameSettings;
class GameState;
class GameType;
class HUD;
class HUDLabel;
class Light;
class Map;
class NetClient;
class Object;
class PhysicsBullet;
class Pickup;
class Player;
class Render;
class Unit;
class UnitType;
class Vehicle;
class Wall;
class WeaponType;
class Weather;
class btVector3;
class EffectsManager;

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
		Weather* weather;
		EffectsManager* effects;

		float time_of_day;			// 0.0 = midnight, 1.0 = midday
		float time_cycle;
		
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
		void removeParticleGroup(SPK::Group* group);

		// Add and remove animations from the renderer
		// This just wraps the render code
		void addAnimPlay(AnimPlay* play, Entity* e);
		void remAnimPlay(AnimPlay* play);

		// Add and remove lights from the renderer
		// This just wraps the render code
		void addLight(Light* light);
		void remLight(Light* light);
		void setTorch(bool on);

		// Debris
		Entity* deadButNotBuried(Entity* e, AnimPlay* play);
		void scatterDebris(Entity* e, unsigned int num, float force, const vector<AssimpModel*>& debris_models);

		// Network bits (EID = entity-id; slots are for players)
		EID getNextEID();
		Entity * getEntity(EID eid);

		// Data queries
		list<UnitQueryResult> * findVisibleUnits(Unit* origin);
		Unit * findUnitSlot(unsigned int slot);
		list<AmmoRound*>* findAmmoRoundsUnit(Unit* u);

		// Get the PlayerState or NULL
		PlayerState* getLocalPlayer(const Player* p);
		PlayerState* getLocalPlayer(unsigned int slot);

		// Start and run
		void preGame();
		void update(float delta);
		void postGame();

		/**
		* The main game loop
		**/
		void gameLoop(Render* render, Audio* audio, NetClient* client);
		void gameLoopIter();

		// Called by Lua, etc
		void gameOver(int result = -1);
		int getLastGameResult();

		// Weapon fun
		vector<WeaponType*>* getSpawnWeapons(UnitType* ut, Faction fac);

		// HUD
		void addHUDMessage(unsigned int slot, string text, string text2 = "");
		HUDLabel* addHUDLabel(unsigned int slot, int x, int y, string data, HUDLabel* l = NULL);
		bool mousePick(unsigned int x, unsigned int y, btVector3& hitLocation, Entity** hitEntity);

	private:
		void doTimeOfDay(float delta);
		void doTorch();
};


GameState * getGameState();
