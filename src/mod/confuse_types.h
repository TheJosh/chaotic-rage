// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "../rage.h"

using namespace std;



/**
* Load a glm::vec3
* The expected config type is CFG_FLOAT_LIST
**/
glm::vec3 cfg_getvec3(cfg_t* cfg, const char* name);
