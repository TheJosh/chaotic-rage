// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
// This file started life as the guichan OpenGL SDL loader
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "../rage.h"
#include "../mod/mod.h"

#include <SDL_image.h>
#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <guichan/opengl.hpp>
#include <guichan/sdl/sdlimageloader.hpp>
#include <guichan/exception.hpp>
#include <guichan/opengl/openglimage.hpp>


namespace gcn
{
	class Image;

	/**
	 * OpenGL ImageLoader that loads images with SDL.
	 */
	class ChaoticRageOpenGLSDLImageLoader : public SDLImageLoader
	{
	private:
		Mod * mod;
		
	public:
		ChaoticRageOpenGLSDLImageLoader(Mod * m)
		{
			this->mod = m;
		}
		
		
		// Inherited from ImageLoader

		virtual Image* load(const std::string& filename, bool convertToDisplayFormat = true)
		{
			SDL_RWops * rw = this->mod->loadRWops(filename);
			if (rw == NULL) {
				throw GCN_EXCEPTION(
						std::string("Unable to load image file: ") + filename);
			}
			
			SDL_Surface *loadedSurface = IMG_Load_RW(rw, 0);
			if (loadedSurface == NULL) {
				throw GCN_EXCEPTION(
						std::string("Unable to load image file: ") + filename);
			}
			
			SDL_Surface *surface = convertToStandardFormat(loadedSurface);
			SDL_FreeSurface(loadedSurface);
			
			if (surface == NULL) {
				throw GCN_EXCEPTION(
						std::string("Not enough memory to load: ") + filename);
			}
			
			OpenGLImage *image = new OpenGLImage((unsigned int*)surface->pixels, surface->w, surface->h, convertToDisplayFormat);
			SDL_FreeSurface(surface);
			
			if (image == NULL) {
				throw GCN_EXCEPTION(
						std::string("Unable to convert image to OpenGL: ") + filename);
			}
			
			return image;
		}
	};
}
