// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "rage.h"
class WeaponType;

/**
* Per-faction game settings
**/
class GameSettingsFaction
{
	public:
		bool spawn_weapons_unit;
		bool spawn_weapons_gametype;
		std::vector<WeaponType*> spawn_weapons_extra;

	public:
		GameSettingsFaction();
};


/**
* Game settings
**/
class GameSettings
{
	public:
		GameSettingsFaction factions[NUM_FACTIONS];
		enum ViewMode {
			behindPlayer,
			abovePlayer,
			firstPerson,
			nrOfViewModes,
		};

		/**
		* Time of day and day-night cycle
		**/
		float time_of_day;
		bool day_night_cycle;

		/**
		* Amount of weather
		**/
		int rain_flow;
		int snow_flow;
		bool random_weather;
		bool gametype_weather;

	public:
		GameSettings(int rounds = 20);

	public:
		void switchViewMode();
		int getRounds() { return rounds; }

	private:
		int rounds;
};
