#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class GameState
{
	private:
		vector<Unit*> units;
		
	public:
		Player* curr_player;
		Map* map;
		unsigned int anim_frame;
		unsigned int game_time;
		
	public:
		GameState();
		~GameState();
		
	public:
		int numUnits();
		void addUnit(Unit* unit);
		Unit* getUnit(int idx);
		
};
