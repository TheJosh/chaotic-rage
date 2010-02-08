#include <iostream>
#include <SDL.h>
#include <zzip/zzip.h>
#include "rage.h"

using namespace std;


static bool load_err = false;


/**
* Loads a sprite from file into memeory
* Uses magneta for colour key
**/
SDL_Surface *loadSprite (const char *filename)
{
	SDL_Surface *sprite;
	Uint32 colourkey;
	SDL_RWops *rw;
	
	rw = SDL_RWFromZZIP(filename, "r");
	if (rw == NULL) {
		fprintf(stderr, "Couldn't load sprite '%s'.\n", filename);
		load_err = true;
		return NULL;
	}
	
	sprite = SDL_LoadBMP_RW(rw, 0);
	SDL_FreeRW(rw);
	
	if (sprite == NULL) {
		fprintf(stderr, "Couldn't load sprite '%s'.\n", filename);
		load_err = true;
		return NULL;
	}
	
	if (sprite->format->BitsPerPixel != 24) {
		DEBUG("Bitmap '%s' not in 24-bit colour; may have problem with colour-key\n", filename);
	}
	
	sprite = SDL_DisplayFormat(sprite);
	
	colourkey = SDL_MapRGB(sprite->format, 255, 0, 255);
	SDL_SetColorKey(sprite, SDL_SRCCOLORKEY, colourkey);
	
	return sprite;
}


bool wasLoadSpriteError()
{
	return load_err;
}

