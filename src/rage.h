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
	#define VERSION "Beta 14"
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
class AssimpNode;
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

#include "util/obj.h"
#include "util/sdl_util.h"
#include "util/SDL_rwops_zzip.h"
#include "util/util.h"
#include "util/debug.h"
#include "util/cmdline.h"
#include "util/clientconfig.h"
#include "util/serverconfig.h"
#include "util/ui_update.h"

#include "render/render.h"
#include "render/render_3d.h"
#include "render/render_opengl.h"
#include "render/render_opengl_compat.h"
#include "render/render_null.h"
#include "render/render_debug.h"
#include "render/hud.h"
#include "render/animplay.h"
#include "render/assimpmodel.h"

#include "audio/audio.h"
#include "audio/audio_sdlmixer.h"
#include "audio/audio_null.h"

#include "physics_bullet.h"

#include "net/net.h"
#include "net/net_client.h"
#include "net/net_server.h"
#include "net/net_gameinfo.h"

#include "mod/mod.h"
#include "mod/mod_proxy.h"
#include "mod/mod_manager.h"
#include "mod/aitype.h"
#include "mod/campaign.h"
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

#include "entity.h"
#include "unit.h"
#include "player.h"
#include "npc.h"
#include "object.h"
#include "newparticle.h"
#include "vehicle.h"
#include "wall.h"
#include "decaying.h"

#include "map.h"
#include "zone.h"

#include "gui/dialog.h"
#include "gui/controls.h"
#include "gui/network.h"
#include "gui/newgame.h"
#include "gui/newcampaign.h"
#include "gui/settings.h"

#include "gamestate.h"
#include "game.h"
#include "intro.h"
#include "menu.h"


