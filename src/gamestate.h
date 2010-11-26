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
	friend class AudioSDLMixer;
	
	protected:
		vector<Entity*> entities;
		vector<Entity*> entities_add;
		vector<Particle*> particles;
		vector<Mod*> mods;
		
	public:
		Map* map;
		
		Player* curr_player;
		
		unsigned int anim_frame;
		unsigned int game_time;
		
		Render* render;
		HUD* hud;
		Audio* audio;
		
	public:
		GameState();
		~GameState();
		
	public:
		void addUnit(Unit* unit);
		void addParticle(Particle* particle);
		void addParticleGenerator(ParticleGenerator* generator);
		void addWall(Wall* wall);
		
		vector<Particle*> * particlesInside(int x, int y, int w, int h);
		Wall * checkHitWall(float x, float y, int check_radius);
		
		void update(int delta);
		
		void addMod(Mod * mod);
		Mod * getMod(int id);
};


GameState * getGameState();


