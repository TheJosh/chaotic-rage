// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <SDL.h>
#include "../rage.h"
#include "../map.h"
#include <map>

#ifndef O_BINARY
#define O_BINARY 0
#endif


using namespace std;


class AIType;
class Campaign;
class FloorType;
class GameType;
class ObjectType;
class PickupType;
class Song;
class Sound;
class UnitType;
class VehicleType;
class WallType;
class WeaponType;
class AssimpModel;


class Mod {
	private:
		// metadata
		string title;
		vector<MapReg> * maps;
		bool has_arcade;
		bool has_campaign;
		
		// full data
		vector<AIType*> * ais;
		vector<Campaign*> * campaigns;
		vector<FloorType*> * areatypes;
		vector<GameType*> * gametypes;
		vector<ObjectType*> * objecttypes;
		vector<PickupType*> * pickuptypes;
		vector<Song*> * songs;
		vector<Sound*> * sounds;
		vector<UnitType*> * unitclasses;
		vector<VehicleType*> * vehicletypes;
		vector<WallType*> * walltypes;
		vector<WeaponType*> * weapontypes;

		map<string, AssimpModel*> models;

	public:
		GameState * st;
		string name;
		string directory;
		
	public:
		virtual char* loadText(string filename);
		virtual Uint8* loadBinary(string resname, int * len);
		virtual SDL_RWops* loadRWops(string filename);
		
	public:
		Mod(GameState * st, string directory);
		~Mod();

	public:
		bool loadMetadata();
		bool load(UIUpdate* ui);
		bool reloadAttrs();
		
		string getName() { return this->name; }
		string getTitle() { return this->title; }
		vector<MapReg>* getMaps() { return this->maps; }
		bool hasArcade() { return this->has_arcade; }
		bool hasCampaign() { return this->has_campaign; }
		
		AIType * getAIType(CRC32 id);
		ObjectType * getObjectType(CRC32 id);
		PickupType * getPickupType(CRC32 id);
		UnitType * getUnitType(CRC32 id);
		VehicleType * getVehicleType(CRC32 id);
		WallType * getWallType(CRC32 id);
		WeaponType * getWeaponType(CRC32 id);
		
		AIType * getAIType(string name);
		Campaign * getCampaign(string name);
		GameType * getGameType(string name);
		ObjectType * getObjectType(string name);
		PickupType * getPickupType(string name);
		Song * getSong(string name);
		Sound * getSound(string name);
		UnitType * getUnitType(string name);
		VehicleType * getVehicleType(string name);
		WallType * getWallType(string name);
		WeaponType * getWeaponType(string name);
		
		AssimpModel * getAssimpModel(string name);
		
		vector<Campaign*> * getCampaigns();

		void addObjectType(ObjectType * ot);
		
		Song * getRandomSong();

		void getAllGameTypes(vector<GameType*>::iterator * start, vector<GameType*>::iterator * end);
		void getAllUnitTypes(vector<UnitType*>::iterator * start, vector<UnitType*>::iterator * end);
};
