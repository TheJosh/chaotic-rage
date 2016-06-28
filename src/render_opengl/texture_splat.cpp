// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;


#include "texture_splat.h"



TextureSplat::TextureSplat()
{
	this->alphamap = NULL;
	for (unsigned int i = 0; i < TEXTURE_SPLAT_LAYERS; ++i) {
		this->layers[i] = NULL;
	}
}


TextureSplat::~TextureSplat()
{
	// TODO: How to free sprites without a ptr to the renderer?
}


void TextureSplat::bindTextures()
{
	for (unsigned int i = 0; i < TEXTURE_SPLAT_LAYERS; ++i) {
		if (this->layers[i] == NULL) break;
		glActiveTexture(GL_TEXTURE5 + i);
		glBindTexture(GL_TEXTURE_2D, this->layers[i]->pixels);
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->alphamap->pixels);
}