#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


GameState::GameState()
{
	this->anim_frame = 0;
	this->game_time = 0;
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
