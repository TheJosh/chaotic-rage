// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "game_state.h"


/**
* TODO: Rename this file to events.h or maybe rejig the events system
**/


// Mouse movement, including keyboard simulation
extern bool ignore_relative_mouse[MAX_LOCAL];
extern int game_x[MAX_LOCAL], game_y[MAX_LOCAL];
extern int net_x[MAX_LOCAL], net_y[MAX_LOCAL];
extern int mk_down_x[MAX_LOCAL], mk_down_y[MAX_LOCAL];

/**
* Handles local events (keyboard, mouse)
* Located in events.cpp
**/
void handleEvents(GameState *st);

