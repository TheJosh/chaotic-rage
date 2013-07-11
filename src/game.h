// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "gamestate.h"


// Mouse movement, including keyboard simulation
extern int game_x[MAX_LOCAL], game_y[MAX_LOCAL];
extern int net_x[MAX_LOCAL], net_y[MAX_LOCAL];
extern int mk_down_x[MAX_LOCAL], mk_down_y[MAX_LOCAL];


/**
* The main game loop
* Located in game.cpp
**/
void gameLoop(GameState *st, Render *render);

/**
* Handles local events (keyboard, mouse)
* Located in events.cpp
**/
void handleEvents(GameState *st);

