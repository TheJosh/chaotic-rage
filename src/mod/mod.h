// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <SDL.h>
#include "../rage.h"
#include "../map/reg.h"
#include "../audio/audio.h"
#include <map>

#ifndef O_BINARY
#define O_BINARY 0
#endif


// We define the maximum size for an individual file
// in a mod as being no larger than this
// It's a protection against very badly compressed assets
#define MAX_FILE_SIZE (32 * 1024 * 1024)


using namespace std;


class AIType;
class Campaign;
class GameType;
class ObjectType;
class PickupType;
class Song;
class UnitType;
class VehicleType;
class WallType;
class WeaponType;
class WeaponAttachment;
class AssimpModel;


class Mod {
	private:
		// metadata
		string title;

		// menu stuff
		vector<MapReg> * maps;
		bool has_arcade;
		bool has_campaign;
		string menu_model;

		// full data
		vector<AIType*> * ais;
		vector<Campaign*> * campaigns;
		vector<GameType*> * gametypes;
		vector<ObjectType*> * objecttypes;
		vector<PickupType*> * pickuptypes;
		vector<Song*> * songs;
		vector<UnitType*> * unitclasses;
		vector<VehicleType*> * vehicletypes;
		vector<WallType*> * walltypes;
		vector<WeaponType*> * weapontypes;
		vector<WeaponAttachment*> * weaponattachments;

		// Caches
		map<string, AssimpModel*> models;
		map<string, AudioPtr> sounds;

		// What went wrong
		string load_err;

	public:
		GameState * st;
		string name;
		string directory;

	public:
		virtual char* loadText(string filename);
		virtual Uint8* loadBinary(string resname, Sint64 *len);
		virtual SDL_RWops* loadRWops(string filename);
		virtual std::string getRealFilename(std::string resname);

	public:
		Mod(GameState * st, string directory);
		virtual ~Mod();

	public:
		bool loadMetadata();
		bool load(UIUpdate* ui);
		void setLoadErr(const char *message, ...);
		string getLoadErr() { return this->load_err; }
		bool reloadAttrs();

		string getName() { return this->name; }
		string getTitle() { return this->title; }
		vector<MapReg>* getMaps() { return this->maps; }
		bool hasArcade() { return this->has_arcade; }
		bool hasCampaign() { return this->has_campaign; }
		string getMenuModelName() { return this->menu_model; }

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
		UnitType * getUnitType(string name);
		VehicleType * getVehicleType(string name);
		WallType * getWallType(string name);
		WeaponType * getWeaponType(string name);
		WeaponAttachment * getWeaponAttachment(string name);

		AssimpModel * getAssimpModel(string filename);
		AssimpModel * getAssimpModel(string filename, AssimpLoadFlags flags);
		AudioPtr getSound(string filename);

		vector<Campaign*> * getCampaigns();

		void addObjectType(ObjectType * ot);

		Song * getRandomSong();

		void getAllGameTypes(vector<GameType*>::iterator * start, vector<GameType*>::iterator * end);
		void getAllUnitTypes(vector<UnitType*>::iterator * start, vector<UnitType*>::iterator * end);
		void getAllWeaponTypes(vector<WeaponType*>::iterator * start, vector<WeaponType*>::iterator * end);
};
