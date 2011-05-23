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
	friend class AudioSDLMixer;
	
	protected:
		list<Entity*> entities;
		vector<Entity*> entities_add;
		vector<Mod*> mods;
		
		vector<Unit*> units;		// leaks: items are not removed
		vector<Wall*> walls;		// leaks: items are not removed
		
		list<CollideBox*> collideboxes;		// hmm
		
	public:
		Map* curr_map;
		Player* curr_player;
		
		unsigned int anim_frame;
		unsigned int game_time;
		
		Render* render;
		HUD* hud;
		Audio* audio;
		GameLogic* logic;
		MapGrid* collides;
		bool reset_mouse;
		
	public:
		GameState();
		~GameState();
		
	public:
		void addUnit(Unit* unit);
		void addParticle(Particle* particle);
		void addParticleGenerator(ParticleGenerator* generator);
		void addWall(Wall* wall);
		
		Wall * checkHitWall(float x, float y, int check_radius);
		Unit * checkHitUnit(float x, float y, int check_radius);
		
		void start();
		void update(int delta);
		
		void addMod(Mod * mod);
		Mod * getMod(int id);
		
		CollideBox* addCollideBox(int x, int y, int radius, Entity *e, bool cares);
		void moveCollideBox(CollideBox* box, int x, int y);
		void sizeCollideBox(CollideBox* box, int radius);
		void delCollideBox(CollideBox* box);
		
	private:
		void doCollisions();
};


GameState * getGameState();


