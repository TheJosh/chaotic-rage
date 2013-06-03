// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once

#include "platform/platform.h"
#include <iostream>
#include <SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <vector>
#include <list>


// Engine features to optionally compile into the game
#ifdef RELEASE
	#define VERSION "Beta 17"
#else
	#define DEBUG_OPTIONS
	#define VERSION "Git Master"
#endif

// Frames-per-second to run all in-game animations at.
// Specified as a float to help out some arithmetic
#define ANIMATION_FPS	12.0



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


#include "util/_util.h"
#include "render/_render.h"
#include "audio/_audio.h"

#include "physics_bullet.h"

#include "net/_net.h"

#include "mod/_mod.h"

#include "weapons/weapons.h"

#include "lua/gamelogic.h"
#include "lua/ailogic.h"

#include "entity/_entity.h"

#include "fx/newparticle.h"

#include "map.h"
#include "zone.h"

#include "gui/_gui.h"

#include "gamestate.h"
#include "game.h"
#include "intro.h"
#include "menu.h"


