// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
// This file started life as the guichan OpenGL SDL loader
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "../rage.h"
#include "../mod/mod.h"

#include "../guichan/guichan.hpp"
#include "../guichan/sdl.hpp"
#include "../guichan/opengl.hpp"
#include "../guichan/exception.hpp"
#include "../guichan/sdl/sdlimageloader.hpp"
#include "../guichan/opengl/openglimage.hpp"


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

		virtual Image* load(const std::string& filename, bool convertToDisplayFormat = true);
	};
}
