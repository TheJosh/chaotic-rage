// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <map>
#include "reg.h"
#include "../mod/mod.h"

using namespace std;


/**
* Find maps in a given directory and add them to the map registry
**/
void MapRegistry::find(string dir)
{
	maps.push_back(MapReg("therlor_valley", "Therlor Valley"));
	maps.push_back(MapReg("lakeside", "Lakeside"));
	maps.push_back(MapReg("stormy_desert", "Stormy Desert"));
	maps.push_back(MapReg("caves", "Caves test"));
}


/**
* Find maps which are contained within a specified mod
**/
void MapRegistry::find(Mod* mod)
{
	mod->loadMetadata();
	vector<MapReg>* maps = mod->getMaps();

	for (vector<MapReg>::iterator it = maps->begin(); it != maps->end(); ++it) {
		this->maps.push_back(*it);
	}
}


/**
* Find a map, by name
**/
MapReg * MapRegistry::get(string name)
{
	for (vector<MapReg>::iterator it = maps.begin(); it != maps.end(); ++it) {
		if (it->getName() == name) return &(*it);
	}
	return NULL;
}

