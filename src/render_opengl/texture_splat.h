// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once

#include "../render/sprite.h"


#define TEXTURE_SPLAT_LAYERS 4


class TextureSplat {
	public:
		SpritePtr alphamap;
		SpritePtr layers[TEXTURE_SPLAT_LAYERS];

	public:
		TextureSplat();
		~TextureSplat();

};
