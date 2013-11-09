// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once

#include "platform/platform.h"
#include <vector>
#include <list>
#include <string>


// Engine features to optionally compile into the game
#ifdef RELEASE
	#define VERSION "1.3"
#else
	#define DEBUG_OPTIONS
	#define VERSION "Git Master"
#endif

// Frames-per-second to run all in-game animations at.
// Specified as a float to help out some arithmetic
#define ANIMATION_FPS	12.0



#include <stdint.h>
typedef int16_t		Sint16;
typedef uint16_t	Uint16;
typedef int32_t		Sint32;
typedef uint32_t	Uint32;
typedef uint32_t	CRC32;


// Some values for things are a randomly chosen between a min and a max
typedef struct range {
	int min;
	int max;
} Range;

class RangeF {
	public:
		float min;
		float max;
		RangeF() { min = 0.0f, max = 0.0f; }
};


class AIType;
class AILogic;
class AnimPlay;
class AssimpMaterial;
class AssimpModel;
class AssimpAnimation;
class AssimpNode;
class AssimpNodeAnim;
class AssimpAnimKey;
class AmmoRound;
class Audio;
class CampaignStage;
class Campaign;
class CommandLineArgs;
class ClientConfig;
class Decaying;
class Dialog;
class DialogNewGame;
class DialogNull;
class DialogControls;
class Entity;
class FloorType;
class GameLogic;
class GameState;
class GameType;
class HUD;
class HUDLabel;
class Intro;
class Map;
class MapMesh;
class MapRegistry;
class MapReg;
class Menu;
class Mod;
class ModProxy;
class ModManager;
class NetClient;
class NetServer;
class NewParticle;
class NPC;
class Object;
class ObjectType;
class PickupType;
class Pickup;
class PhysicsBullet;
class Player;
class PlayerState;
class Render;
class Render3D;
class Song;
class Sound;
class ServerConfig;
class UIUpdate;
class Unit;
class UnitType;
class VehicleType;
class Vehicle;
class Wall;
class WallType;
class WeaponType;
class WeaponRaycast;
class WeaponDigdown;
class WeaponFlamethrower;
class WavefrontObj;
class Zone;


#include "util/util.h"
#include "util/debug.h"





