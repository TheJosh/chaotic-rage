// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <SDL.h>
#include <SDL_net.h>
#include <vector>

using namespace std;


class UIUpdate;


/**
* Returns a list of local server names
* Don't forget to delete() the result!
**/
vector<string>* getLocalServers(UIUpdate * ui);
