// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once

#include "platform/platform.h"
#include "types.h"
#include <vector>
#include <list>
#include <string>

// Bullet math library
#include <LinearMath/btQuaternion.h>
#include <LinearMath/btTransform.h>
#include <LinearMath/btVector3.h>
#include <LinearMath/btScalar.h>


// Engine features to optionally compile into the game
#ifdef RELEASE
	#define VERSION "1.14"
#else
	#define DEBUG_OPTIONS
	#define VERSION "Git Master"
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

// Suppress stupid compiler warning in Bullet 2.82
#if (BT_BULLET_VERSION == 282)
inline int btGetInfinityMask() { return btInfinityMask; }
#endif

#include "util/util.h"
#include "util/debug.h"
