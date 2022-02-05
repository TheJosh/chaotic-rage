// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once

#include <string>
#include "./gl.h"

class Mod;
class SDL_Surface;


/**
* VAO wrapper with software fallback
**/
class Texture2DArray
{
	public:
		Texture2DArray();
		~Texture2DArray();

		/**
		* Load a whole array from a single image
		* Array layers should be stacked vertically.
		**/
		void loadSingleImage(std::string filename, Mod* mod, unsigned int numLayers);

		inline GLuint getTexIndex() {
			return _texIndex;
		}

	protected:
		/**
		* Load image data from a file
		**/
		SDL_Surface* rawLoadImage(std::string filename, Mod* mod);
		
		/**
		* Free the memory loaded by rawLoadImage
		**/
		void freeRawImage(SDL_Surface* surf);
		
	private:
		GLuint _texIndex;

};
