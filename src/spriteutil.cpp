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
SDL_Surface *loadSprite (string filename)
{
	SDL_Surface *sprite;
	Uint32 colourkey;
	SDL_RWops *rw;
	
	rw = SDL_RWFromZZIP(filename.c_str(), "r");
	if (rw == NULL) {
		fprintf(stderr, "Couldn't load sprite '%s'.\n", filename.c_str());
		load_err = true;
		return NULL;
	}
	
	sprite = SDL_LoadBMP_RW(rw, 0);
	SDL_FreeRW(rw);
	
	if (sprite == NULL) {
		fprintf(stderr, "Couldn't load sprite '%s'.\n", filename.c_str());
		load_err = true;
		return NULL;
	}
	
	if (sprite->format->BitsPerPixel != 24) {
		DEBUG("Bitmap '%s' not in 24-bit colour; may have problem with colour-key\n", filename.c_str());
	}
	
	sprite = SDL_DisplayFormat(sprite); // leak !!
	
	colourkey = SDL_MapRGB(sprite->format, 255, 0, 255);
	SDL_SetColorKey(sprite, SDL_SRCCOLORKEY, colourkey);
	
	return sprite;
}


bool wasLoadSpriteError()
{
	return load_err;
}


/**
* Tiles a sprite
**/
SDL_Surface *tileSprite (SDL_Surface *orig, int w, int h)
{
	SDL_Surface* surf = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32, 0,0,0,0);
	
	for (int x = 0; x <= w; x += orig->w) {
		for (int y = 0; y <= h; y += orig->h) {
			SDL_Rect dest;
			
			dest.x = x;
			dest.y = y;
			
			SDL_BlitSurface(orig, NULL, surf, &dest);
		}
	}
	
	return surf;
}

