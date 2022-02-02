// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once


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

// Forward decls - SPARK particles
namespace SPK {
	class System;
	class Group;
	class Renderer;

	namespace GL {
		class GLRenderer;
	}
}

// Factions that units can be in
// A unit can only be in one faction at any given time
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
	NUM_FACTIONS,
};

// Dialog names/identifications
enum DialogName {
	BUTTONBAR,
	CAMPAIGN,
	CLIENT_SETTINGS,
	CONTROLS,
	GAME,
	MODS,
	NETWORK,
	QUIT,
	TEXTBOX,
	TEXTPROMPT,
	LISTPROMPT,
	WEAPONS,
	ENVIRONMENT,
};

// Sound pointers
struct Mix_Chunk;
typedef Mix_Chunk* AudioPtr;
