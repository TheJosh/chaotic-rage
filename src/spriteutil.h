#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


SDL_Surface *loadSprite (const char *filename);

bool wasLoadSpriteError();
