// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once


class Unit;
class PickupType;



/**
* The currently active pickups
**/
class UnitPickup
{
	public:
		Unit* u;
		PickupType* pt;
		unsigned int end_time;
};
