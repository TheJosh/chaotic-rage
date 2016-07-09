// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;


#include "texture_splat.h"
#include "glshader.h"



TextureSplat::TextureSplat()
{
	this->alphamap = NULL;
	for (unsigned int i = 0; i < TEXTURE_SPLAT_LAYERS; ++i) {
		this->layers[i].texture = NULL;
		this->layers[i].scale = 100.0f;
		this->layers[i].detail = NULL;
	}
}


TextureSplat::~TextureSplat()
{
	// TODO: How to free sprites without a ptr to the renderer?
}


void TextureSplat::bindTextures()
{
	for (unsigned int i = 0; i < TEXTURE_SPLAT_LAYERS; ++i) {
		if (this->layers[i].texture == NULL) break;
		glActiveTexture(GL_TEXTURE5 + i);
		glBindTexture(GL_TEXTURE_2D, this->layers[i].texture->pixels);
	}
	for (unsigned int i = 0; i < TEXTURE_SPLAT_LAYERS; ++i) {
		if (this->layers[i].detail == NULL) break;
		glActiveTexture(GL_TEXTURE10 + i);
		glBindTexture(GL_TEXTURE_2D, this->layers[i].detail->pixels);
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->alphamap->pixels);
}

