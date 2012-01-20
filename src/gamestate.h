// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


#define MAX_LOCAL 4


class UnitQueryResult
{
	public:
		Unit * u;
		float dist;
};

class GameState
{
	friend class RenderSDL;
	friend class RenderOpenGL;
	friend class RenderNull;
	friend class RenderDebug;
	friend class AudioSDLMixer;

	protected:
		list<Entity*> entities;
		list<Entity*> entities_add;
		list<CollideBox*> collideboxes;
		
		vector<Unit*> units;		// leaks: items are not removed
		vector<Wall*> walls;		// leaks: items are not removed
		
	public:
		Map* curr_map;
		bool reset_mouse;
		
		Player* local_players[MAX_LOCAL];
		unsigned int num_local;
		
		unsigned int entropy;		// TODO: gamestate -> localplayers
		
		int curr_slot;
		
		unsigned int anim_frame;
		unsigned int game_time;
		
		Render* render;
		HUD* hud;					// TODO: gamestate -> localplayers
		Audio* audio;
		GameLogic* logic;
		MapGrid* collides;			// TODO: newphysics
		NetClient* client;
		NetServer* server;
		PhysicsBullet* physics;
		CommandLineArgs* cmdline;
		ModManager* mm;

	public:
		GameState();
		~GameState();
		
	public:
		// Adding
		void addUnit(Unit* unit);
		void addParticle(Particle* particle);
		void addParticleGenerator(ParticleGenerator* generator);
		void addWall(Wall* wall);
		void addObject(Object* object);
		
		// For network
		Unit * findUnitSlot(int slot);
		
		// Start and run
		void start();
		void update(int delta);
		void clear();
		
		// Entropy
		unsigned int getEntropy(unsigned int slot);
		void increaseEntropy(unsigned int slot);
		
		// Data queries
		list<UnitQueryResult> * findVisibleUnits(Unit* origin);
};


GameState * getGameState();


