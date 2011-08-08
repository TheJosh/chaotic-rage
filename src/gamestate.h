// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


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
		
		vector<Mod*> mods;
		
		vector<Unit*> units;		// leaks: items are not removed
		vector<Wall*> walls;		// leaks: items are not removed
		
	public:
		Map* curr_map;
		bool reset_mouse;
		
		Player* curr_player;
		int curr_slot;
		
		unsigned int anim_frame;
		unsigned int game_time;
		
		Render* render;
		HUD* hud;
		Audio* audio;
		GameLogic* logic;
		MapGrid* collides;
		NetClient* client;
		NetServer* server;
		
	public:
		GameState();
		~GameState();
		
	public:
		void addUnit(Unit* unit);
		void addParticle(Particle* particle);
		void addParticleGenerator(ParticleGenerator* generator);
		void addWall(Wall* wall);
		
		Unit * findUnitSlot(int slot);
		
		void start();
		void update(int delta);
		void clear();
		
		void addMod(Mod * mod);
		Mod * getMod(string name);
		Mod * getDefaultMod();
		
		CollideBox* addCollideBox(int x, int y, int radius, Entity *e, bool cares);
		void moveCollideBox(CollideBox* box, int x, int y);
		void sizeCollideBox(CollideBox* box, int radius);
		void delCollideBox(CollideBox* box);
		
		bool inBounds(float x, float y);
		
	private:
		void doCollisions();
};


GameState * getGameState();


