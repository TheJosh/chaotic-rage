#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class GameState
{
	friend void render(GameState *st, SDL_Surface *screen);
	
	protected:
		vector<Unit*> units;
		vector<Particle*> particles;
		vector<ParticleGenerator*> particlegenerators;
		
	public:
		Player* curr_player;
		Map* map;
		unsigned int anim_frame;
		unsigned int game_time;
		HUD* hud;
		
	public:
		GameState();
		~GameState();
		
	public:
		void addUnit(Unit* unit);
		void addParticle(Particle* particle);
		void addParticleGenerator(ParticleGenerator* generator);
		
	public:
		void update(int delta);
};
