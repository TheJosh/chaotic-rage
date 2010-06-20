#pragma once

#include <iostream>
#include <SDL.h>
#include <vector>

#ifdef DEBUG_MODE
	#define WHERESTR "%s:%d\t"
	#define WHEREARG __FILE__, __LINE__
	#define DEBUGPRINT2(...) fprintf(stderr, __VA_ARGS__)
	#define DEBUG(_fmt, ...) DEBUGPRINT2(WHERESTR _fmt, WHEREARG, __VA_ARGS__)
#else
	#define DEBUG(_fmt, ...) /* nothing! */
#endif

typedef struct range {
	int min;
	int max;
} Range;

class Action;
class EntityType;
class UnitClass;
class AreaType;
class ParticleType;
class ParticleGenType;
class WeaponType;
class Entity;
class Unit;
class Player;
class Particle;
class ParticleGenerator;
class Map;
class HUD;
class GameState;

#include "actions.h"
#include "entitytype.h"
#include "areatypes.h"
#include "particletypes.h"
#include "unitclass.h"
#include "weapontypes.h"

#include "entity.h"
#include "unit.h"
#include "player.h"
#include "particle.h"
#include "particlegenerator.h"

#include "map.h"
#include "gamestate.h"

#include "game.h"
#include "video.h"
#include "hud.h"
#include "util.h"
#include "spriteutil.h"

#include "SDL_rwops_zzip.h"
#include "SDL_rotozoom.h"


// Frames-per-second to run all in-game animations at.
// Specified as a float to help out some arithmetic
#define ANIMATION_FPS	6.0

