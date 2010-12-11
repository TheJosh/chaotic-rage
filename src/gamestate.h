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
		vector<Entity*> entities;
		vector<Entity*> entities_add;
		vector<Mod*> mods;
		
		vector<Unit*> units;		// leaks: items are not removed
		vector<Wall*> walls;		// leaks: items are not removed
		
	public:
		Map* map;
		
		Player* curr_player;
		
		unsigned int anim_frame;
		unsigned int game_time;
		
		Render* render;
		HUD* hud;
		Audio* audio;
		GameLogic* logic;
		
	public:
		GameState();
		~GameState();
		
	public:
		void addUnit(Unit* unit);
		void addParticle(Particle* particle);
		void addParticleGenerator(ParticleGenerator* generator);
		void addWall(Wall* wall);
		
		Unit * checkHitUnit(float x, float y, int check_radius);
		Wall * checkHitWall(float x, float y, int check_radius);
		
		void update(int delta);
		
		void addMod(Mod * mod);
		Mod * getMod(int id);
		
	private:
		void doCollisions();
};


GameState * getGameState();


