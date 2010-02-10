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
		
	public:
		GameState();
		~GameState();
		
	public:
		int numUnits();
		void addUnit(Unit* unit);
		Unit* getUnit(int idx);
		
};
