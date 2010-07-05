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
	
	protected:
		vector<Entity*> entities;
		vector<Entity*> entities_add;
		//vector<Unit*> units;
		vector<Particle*> particles;
		//vector<ParticleGenerator*> particlegenerators;
		
	public:
		Player* curr_player;
		Map* map;
		unsigned int anim_frame;
		unsigned int game_time;
		HUD* hud;
		Render* render;
		
	public:
		GameState();
		~GameState();
		
	public:
		void addUnit(Unit* unit);
		void addParticle(Particle* particle);
		void addParticleGenerator(ParticleGenerator* generator);
		
		vector<Particle*> * particlesInside(int x, int y, int w, int h);
		
		void update(int delta);
};
