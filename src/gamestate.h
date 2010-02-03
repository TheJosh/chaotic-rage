#include <iostream>
#include <vector>
#include <SDL.h>
#include "unit.h"
#pragma once

using namespace std;


class GameState
{
	private:
		vector<Unit> units;
		
	public:
		GameState();
		~GameState();
		
	public:
		int numUnits();
		void addUnit(Unit* unit);
		Unit* getUnit(int idx);
		
};
