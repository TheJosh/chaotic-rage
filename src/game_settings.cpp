// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "game_settings.h"

using namespace std;


/**
* New, blank game settings
**/
GameSettings::GameSettings()
{
}

/**
* New, blank per-faction game settings
**/
GameSettingsFaction::GameSettingsFaction()
{
	this->spawn_weapons_unit = true;
	this->spawn_weapons_gametype = true;
}