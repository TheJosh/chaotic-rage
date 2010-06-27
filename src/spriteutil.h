#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


SpritePtr tileSprite (SpritePtr orig, int w, int h);

void cross_mask (SpritePtr dest, SpritePtr mask);

Uint32 getPixel(SDL_Surface *surface, int x, int y);
void setPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
