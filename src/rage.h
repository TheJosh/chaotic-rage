// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once

#include <iostream>
#include <SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <vector>

#ifdef DEBUG_MODE
	#define WHERESTR "%s:%d\t"
	#define WHEREARG __FILE__, __LINE__
	#define DEBUGPRINT2(...) fprintf(stderr, __VA_ARGS__)
	#define DEBUG(_fmt, ...) DEBUGPRINT2(WHERESTR _fmt, WHEREARG, __VA_ARGS__)
#else
	#define DEBUG(_fmt, ...) /* nothing! */
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


class AnimModel;
class AnimPlay;
class FloorType;
class Audio;
class CollideBox;
class Entity;
class EntityType;
class Event;
class EventListener;
class GameLogic;
class GameState;
class HUD;
class Map;
class MapGrid;
class MapGridCell;
class Mod;
class NPC;
class Particle;
class ParticleGenerator;
class ParticleGenType;
class ParticleType;
class Player;
class Render;
class Song;
class Sound;
class Unit;
class UnitType;
class Wall;
class WallType;
class WeaponType;
class Zone;

#include "mod.h"
#include "event.h"

#include "objload.h"
#include "render.h"
#include "render_opengl.h"
#include "render_null.h"

#include "audio.h"
#include "audio_sdlmixer.h"
#include "audio_null.h"

#include "entitytype.h"

#include "animmodel.h"
#include "animplay.h"
#include "floortype.h"
#include "particletype.h"
#include "unittype.h"
#include "song.h"
#include "sound.h"
#include "walltype.h"
#include "weapontype.h"

#include "entity.h"
#include "unit.h"
#include "player.h"
#include "npc.h"
#include "particle.h"
#include "particlegenerator.h"
#include "wall.h"

#include "zone.h"
#include "mapgrid.h"
#include "map.h"
#include "gamelogic.h"
#include "gamestate.h"

#include "game.h"
#include "hud.h"
#include "util.h"

#include "sdl_util.h"
#include "SDL_rwops_zzip.h"
#include "SDL_rotozoom.h"

