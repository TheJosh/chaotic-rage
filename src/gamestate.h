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
		
	public:
		Player* curr_player;
		Map* map;
		unsigned int anim_frame;
		unsigned int game_time;
		
	public:
		GameState();
		~GameState();
		
	public:
		void addUnit(Unit* unit);
		void addParticle(Particle* particle);
		
	public:
		void update(int delta);
};
