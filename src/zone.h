// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;

#define NUM_FACTIONS 10

// See the document docs/factions.txt for info.
enum Faction {
	FACTION_INDIVIDUAL = 0,
	FACTION_TEAM1 = 1,
	FACTION_TEAM2 = 2,
	FACTION_TEAM3 = 3,
	FACTION_TEAM4 = 4,
	FACTION_TEAM5 = 5,
	FACTION_TEAM6 = 6,
	FACTION_TEAM7 = 7,
	FACTION_TEAM8 = 8,
	FACTION_COMMON = 9,
};

class Zone
{
	private:
		int x1;
		int y1;
		int x2;
		int y2;
		
	public:
		// Flags for the various jobs a zone may do
		// A zone may do multiple jobs at once
		// and may do the job for multiple teams
		// expect some (interesting) in-fighting when opposing
		// players are sent to the same prison for example...
		//
		// team 0 is for when not playing team games
		//
		bool spawn[NUM_FACTIONS];      // player spawn points
		bool prison[NUM_FACTIONS];     // team prisons
		bool collect[NUM_FACTIONS];    // team collectables - flags, suitcases, balls, etc.
		bool dest[NUM_FACTIONS];       // where collectables are to be taken
		bool nearbase[NUM_FACTIONS];   // somewhere near the team base - for generic use
	
	public:
		Zone(int point1_x, int point1_y, int point2_x, int point2_y);
		~Zone();
		
	public:
		int getRandomX();
		int getRandomY();
};

