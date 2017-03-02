// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <SDL_image.h>
#include "mod/mod.h"
#include "texture_2d_array.h"
#include "gl_debug.h"


Texture2DArray::Texture2DArray()
{
	glGenTextures(1, &_texIndex);
}

Texture2DArray::~Texture2DArray()
{
	glDeleteTextures(1, &_texIndex);
}


/**
* Load a image in a mod into a SDL_Surface
**/
SDL_Surface* Texture2DArray::rawLoadImage(string filename, Mod* mod)
{
	SDL_Surface* surf;
	SDL_RWops* rw;

	rw = mod->loadRWops(filename);
	if (rw == NULL) {
		return NULL;
	}

	surf = IMG_Load_RW(rw, 0);
	if (surf == NULL) {
		SDL_RWclose(rw);
		return NULL;
	}

	// Store the RWops in a pointer so it can be freed later
	surf->userdata = (void*)rw;

	return surf;
}


/**
* Free the memory loaded by rawLoadImage
**/
void Texture2DArray::freeRawImage(SDL_Surface* surf)
{
	SDL_RWops* rw = (SDL_RWops*) surf->userdata;
	SDL_FreeSurface(surf);
	SDL_RWclose(rw);
}


/**
* Load a whole array from a single image
* Array layers should be stacked vertically.
**/
void Texture2DArray::loadSingleImage(string filename, Mod* mod, unsigned int numLayers)
{
	GLenum texture_format;
	SDL_Surface* surf;

	surf = rawLoadImage(filename, mod);

	if (surf->format->BytesPerPixel == 4) {
		texture_format = GL_RGBA;
	} else {
		texture_format = GL_RGB;
	}

	unsigned int height = surf->h / numLayers;

	glBindTexture(GL_TEXTURE_2D_ARRAY, _texIndex);
	glTexImage3D(
		GL_TEXTURE_2D_ARRAY,
		0, texture_format,
		surf->w, height, numLayers,
		0, texture_format,
		GL_UNSIGNED_BYTE, surf->pixels
	);
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

	freeRawImage(surf);

	CHECK_OPENGL_ERROR;
}
