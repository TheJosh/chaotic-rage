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
		GameState();
		~GameState();
		
	public:
		int numUnits();
		void addUnit(Unit* unit);
		Unit* getUnit(int idx);
		
};
