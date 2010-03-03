#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


SDL_Surface *loadSprite (string filename);
bool wasLoadSpriteError();

SDL_Surface *tileSprite (SDL_Surface *orig, int w, int h);

void cross_mask (SDL_Surface *dest, SDL_Surface *mask);

Uint32 getPixel(SDL_Surface *surface, int x, int y);
void setPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
