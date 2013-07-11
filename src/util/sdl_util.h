// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <SDL.h>
#include "../rage.h"

using namespace std;


SDL_Surface * tileSprite (SDL_Surface * orig, int w, int h);

SDL_Surface* flipVert(SDL_Surface* sfc);

void cross_mask (SDL_Surface * dest, SDL_Surface * mask);

Uint32 getPixel(SDL_Surface *surface, int x, int y);
void setPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
