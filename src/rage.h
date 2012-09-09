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


// Engine features to optionally compile into the game
#ifdef RELEASE
	#define FEAT_INTRO true
	#define FEAT_MENU true
	#define VERSION "Beta 6"
#else
	#define FEAT_INTRO false
	#define FEAT_MENU true
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
class AnimModel;
class AnimPlay;
class Audio;
class CommandLineArgs;
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
class Intro;
class Map;
class Menu;
class Mod;
class ModManager;
class NetClient;
class NetServer;
class NewParticle;
class NPC;
class Object;
class ObjectType;
class PhysicsBullet;
class Player;
class Render;
class Song;
class Sound;
class Unit;
class UnitType;
class VehicleType;
class Vehicle;
class Wall;
class WallType;
class WeaponType;
class WavefrontObj;
class Zone;

#include "util/obj.h"
#include "util/sdl_util.h"
#include "util/SDL_rwops_zzip.h"
#include "util/util.h"
#include "util/debug.h"
#include "util/cmdline.h"

#include "render/render.h"
#include "render/render_opengl.h"
#include "render/render_null.h"
#include "render/render_debug.h"
#include "render/hud.h"

#include "audio/audio.h"
#include "audio/audio_sdlmixer.h"
#include "audio/audio_null.h"

#include "physics_bullet.h"

#include "net/net.h"
#include "net/net_client.h"
#include "net/net_server.h"

#include "mod/mod.h"
#include "mod/mod_manager.h"
#include "mod/aitype.h"
#include "mod/animmodel.h"
#include "mod/animplay.h"
#include "mod/floortype.h"
#include "mod/objecttype.h"
#include "mod/unittype.h"
#include "mod/song.h"
#include "mod/sound.h"
#include "mod/walltype.h"
#include "mod/vehicletype.h"
#include "mod/weapontype.h"
#include "mod/gametype.h"

#include "lua/gamelogic.h"
#include "lua/ailogic.h"

#include "gui/dialog.h"

#include "entity.h"
#include "unit.h"
#include "player.h"
#include "npc.h"
#include "object.h"
#include "newparticle.h"
#include "vehicle.h"
#include "wall.h"

#include "map.h"
#include "zone.h"

#include "gamestate.h"
#include "game.h"
#include "intro.h"
#include "menu.h"


