// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "types.h"
#include <vector>

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
		float getTimeOfDayMax() { return 1.0f; }
		float getTimeOfDayMin() { return 0.0f; }
		float getTimeOfDayStep() { return 0.01f; }
		bool day_night_cycle;

		/**
		* Amount of weather
		**/
		int rain_flow;
		int snow_flow;
		bool random_weather;
		bool gametype_weather;

		GameSettings(int rounds = 10);

		void switchViewMode();
		int getRounds() { return rounds; }

	private:
		int rounds;
};
