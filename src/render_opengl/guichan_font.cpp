// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "../rage.h"
#include "../render/render_3d.h"
#include "guichan_font.h"


using namespace gcn;


/**
* Gets the width of a string. The width of a string is not necesserily
* the sum of all the widths of it's glyphs.
*
* @param text The string to return the width of.
* @return The width of a string.
*/
int ChaoticRageFont::getWidth(const std::string& text) const
{
	return this->render->widthText(text);
}


/**
* Gets the height of the glyphs in the font.
*
* @return The height of the glyphs int the font.
*/
int ChaoticRageFont::getHeight() const
{
	return 20;
}


/**
* Draws a string.
*
* NOTE: You normally won't use this function to draw text since
*       Graphics contains better functions for drawing text.
*
* @param graphics A Graphics object to use for drawing.
* @param text The string to draw.
* @param x The x coordinate where to draw the string.
* @param y The y coordinate where to draw the string.
*/
void ChaoticRageFont::drawString(Graphics* graphics, const std::string& text, int x, int y)
{
	// TODO: Fix crashes
	// this->render->renderText(text, (float)x, (float)y, 0.0f, 0.0f, 0.0f, 0.0f);
}

