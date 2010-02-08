#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


GameState::GameState()
{
}

GameState::~GameState()
{
}


int GameState::numUnits()
{
	return this->units.size();
}

void GameState::addUnit(Unit* unit)
{
	this->units.push_back(unit);
}

Unit* GameState::getUnit(int idx)
{
	return this->units.at(idx);
}
