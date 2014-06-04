// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "render_opengl.h"
#include "../rage.h"
#include "../guichan/guichan.hpp"
#include "../guichan/font.hpp"


namespace gcn
{
	class ChaoticRageFont : public Font
	{
	private:
		RenderOpenGL* render;
		int height;

	public:
		ChaoticRageFont(RenderOpenGL* render, int height)
		{
			this->render = render;
			this->height = height;
		}

		/**
		* Gets the width of a string. The width of a string is not necesserily
		* the sum of all the widths of it's glyphs.
		*
		* @param text The string to return the width of.
		* @return The width of a string.
		*/
		virtual int getWidth(const std::string& text) const;

		/**
		* Gets the height of the glyphs in the font.
		*
		* @return The height of the glyphs int the font.
		*/
		virtual int getHeight() const;

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
		virtual void drawString(Graphics* graphics, const std::string& text, int x, int y);

	};
}

