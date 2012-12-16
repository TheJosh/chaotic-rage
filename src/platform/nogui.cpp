// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

/**
* This file contains stub functions for bits which
* aren't available to non-gui dedicated servers
**/


#include <string>
#include <iostream>
#include "../rage.h"

using namespace std;


/**
* Handles SDL quit events only, no other interaction is suported
**/
void handleEvents(GameState *st)
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			st->running = false;
		}
	}
}

/**
* NULL music loader
**/
Mix_Music * Mix_LoadMUS_RW(SDL_RWops * rwops)
{
	return NULL;
}


/**
* Fake HUD class which does nothing
**/
HUD::HUD(PlayerState *ps) {}
void HUD::addMessage(string text) {}
void HUD::addMessage(string text1, string text2) {}
HUDLabel * HUD::addLabel(int x, int y, string data) { return NULL; }


