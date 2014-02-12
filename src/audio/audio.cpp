// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <algorithm>
#include <SDL.h>
#include "../rage.h"
#include "audio.h"
#include "../game_state.h"


using namespace std;


Audio::Audio(GameState * st)
{
	this->st = st;
}
