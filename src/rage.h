// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once

#include "platform/platform.h"
#include <vector>
#include <list>
#include <string>

// Bullet math library
#include <LinearMath/btQuaternion.h>
#include <LinearMath/btTransform.h>
#include <LinearMath/btVector3.h>


// Engine features to optionally compile into the game
#ifdef RELEASE
	#define VERSION "1.6"
#else
	#define DEBUG_OPTIONS
	#define VERSION "Git Master"
#endif

// For now we aren't going to support Spark particles on mobile devices
#if defined(__ANDROID__)
	#define USE_SPARK 0
#else
	#define USE_SPARK 1
#endif

// Frames-per-second to run all in-game animations at.
// Specified as a float to help out some arithmetic
#define ANIMATION_FPS	12.0

// The ratio of a circle's circumference to its diameter
// Might be useful for something
#define PI 3.1415f

// I'll give you one guess
#define MAX(a,b) ((a) >= (b) ? (a) : (b))
#define MIN(a,b) ((a) <= (b) ? (a) : (b))

// Degrees <-> Radians
#define DEG_TO_RAD(x) ((x) * (PI / 180.0f))
#define RAD_TO_DEG(x) ((x) * (180.0f / PI))

// These are defined by SDL, but we redefine them to save loading SDL
#include <stdint.h>
typedef int8_t      Sint8;
typedef uint8_t     Uint8;
typedef int16_t     Sint16;
typedef uint16_t    Uint16;
typedef int32_t     Sint32;
typedef uint32_t    Uint32;
typedef int64_t     Sint64;
typedef uint64_t    Uint64;

// CRC32s are used for unique IDs for entity types
typedef uint32_t    CRC32;

// Unique IDs for network play
typedef uint16_t    EID;

// Saves loading sprite.h and it's dependencies - SDL and OpenGL
class Sprite;
typedef Sprite* SpritePtr;

// Saves including SDL in headers
struct SDL_RWops;

// Saves including libConfuse in headers
struct cfg_opt_t;
struct cfg_t;

// Some values for things are a randomly chosen between a min and a max (ints)
typedef struct range {
	int min;
	int max;
} Range;

// Some values for things are a randomly chosen between a min and a max (floats)
class RangeF {
	public:
		float min;
		float max;
		RangeF() { min = 0.0f, max = 0.0f; }
};

// A buttload of class definitions for forward-decls.
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

