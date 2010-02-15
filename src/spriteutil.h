#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


SDL_Surface *loadSprite (string filename);
bool wasLoadSpriteError();

SDL_Surface *tileSprite (SDL_Surface *orig, int w, int h);
