#include <iostream>
#include <vector>
#include "gamestate.h"
#include "unit.h"

using namespace std;


GameState::GameState()
{
	return;
}

GameState::~GameState()
{
	return;
}


int GameState::numUnits()
{
	return this->units.size();
}

void GameState::addUnit(Unit* unit)
{
	this->units.push_back(*unit);
}

Unit* GameState::getUnit(int idx)
{
	return &this->units.at(idx);
}
