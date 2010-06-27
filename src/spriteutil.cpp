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
SpritePtr loadSprite (string filename)
{
	SDL_Surface *sprite;
	Uint32 colourkey;
	SDL_RWops *rw;
	
	DEBUG("Loading bitmap '%s'.\n", filename.c_str());
	
	rw = SDL_RWFromZZIP(filename.c_str(), "r");
	if (rw == NULL) {
		fprintf(stderr, "Couldn't load sprite '%s'.\n", filename.c_str());
		load_err = true;
		return NULL;
	}
	
	sprite = SDL_LoadBMP_RW(rw, 0);
	SDL_RWclose (rw);
	
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


/**
* Sprite error
**/
bool wasLoadSpriteError()
{
	return load_err;
}


/**
* Tiles a sprite
**/
SpritePtr tileSprite (SpritePtr orig, int w, int h)
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


/**
* Apply the colourkey mask from one surface onto another
**/
void cross_mask (SpritePtr dest, SpritePtr mask)
{
	SDL_LockSurface(dest);
	SDL_LockSurface(mask);
	
	if (mask->format->BytesPerPixel != dest->format->BytesPerPixel) return;
	
	int bpp = mask->format->BytesPerPixel;
	
	for (int x = 0; x < mask->w; x++) {
		for (int y = 0; y < mask->h; y++) {
			
			Uint8 *pm = (Uint8 *)mask->pixels + y * mask->pitch + x * bpp;
			Uint8 *pd = (Uint8 *)dest->pixels + y * dest->pitch + x * bpp;
			
			if (*(Uint32 *)pm == mask->format->colorkey) {
				*(Uint32 *)pd = *(Uint32 *)pm;
			}
			
		}
	}
	
	SDL_UnlockSurface(dest);
	SDL_UnlockSurface(mask);
	
	SDL_SetColorKey(dest, SDL_SRCCOLORKEY, mask->format->colorkey);
}


/**
* Gets the value of a pixel
* Use SDL_GetRGB to get the red, green, and blue components
**/
Uint32 getPixel(SDL_Surface *surface, int x, int y)
{
	if (x < 0) return 0;
	if (y < 0) return 0;
	if (x > surface->w) return 0;
	if (y > surface->h) return 0;
	
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch(bpp) {
	case 4:
		return *(Uint32 *)p;
		
	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			return p[0] << 16 | p[1] << 8 | p[2];
		} else {
			return p[0] | p[1] << 8 | p[2] << 16;
		}
		
	case 2:
		return *(Uint16 *)p;
		
	case 1:
		return *p;
		
	default:
		return 0;
	}
}


/**
* Changes the value of a pixel
* Use SDL_MapRGB to get a pixel value for a set of R, G, and B values
**/
void setPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	if (x < 0) return;
	if (y < 0) return;
	if (x > surface->w) return;
	if (y > surface->h) return;
	
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to set */
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	
	switch(bpp) {
	case 4:
		*(Uint32 *)p = pixel;
		break;
		
	case 3:
		if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			p[0] = (pixel >> 16) & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = pixel & 0xff;
		} else {
			p[0] = pixel & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = (pixel >> 16) & 0xff;
		}
		break;
		
	case 2:
		*(Uint16 *)p = pixel;
		break;
		
	case 1:
		*p = pixel;
		break;
		
	}
}


