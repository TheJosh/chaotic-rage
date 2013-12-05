// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "../rage.h"
#include "../render_opengl/gl.h"
#include <SDL.h>


using namespace std;


class Sprite {
	public:
		int w;
		int h;
		GLuint pixels;
		SDL_Surface *orig;		// TODO: remove dependency on this...perhaps
};

typedef Sprite* SpritePtr;
