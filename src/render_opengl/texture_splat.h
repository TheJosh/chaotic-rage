// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once

#include "../render/sprite.h"

#define TEXTURE_SPLAT_LAYERS 4

class GLShader;


class TextureSplatLayer
{
	public:
		/**
		* Main texture
		**/
		SpritePtr texture;

		/**
		* Scale for main texture
		**/
		float scale;

		/**
		* Should this texture be doubled up?
		**/
		bool dbl;
};


class TextureSplat
{
	public:
		SpritePtr alphamap;
		TextureSplatLayer layers[TEXTURE_SPLAT_LAYERS];

	public:
		TextureSplat();
		~TextureSplat();

		void bindTextures();
};
