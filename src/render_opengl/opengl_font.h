// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "../types.h"
#include <guichan/guichan.hpp>
#include <guichan/font.hpp>


class OpenGLFont_Implementation;
class RenderOpenGL;
class Mod;


class OpenGLFont : public gcn::Font
{
	private:
		RenderOpenGL* render;
		float size;
		OpenGLFont_Implementation *pmpl;

	public:
		OpenGLFont(RenderOpenGL* render, std::string name, Mod* mod, float size);
		~OpenGLFont();

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
		virtual void drawString(gcn::Graphics* graphics, const std::string& text, int x, int y);
		
		/**
		* Draws a string.
		*
		* @param graphics A Graphics object to use for drawing.
		* @param text The string to draw.
		* @param x The x coordinate where to draw the string.
		* @param y The y coordinate where to draw the string.
		* @param r Colour - red
		* @param g Colour - green
		* @param b Colour - blue
		* @param a Colour - alpha
		*/
		void drawString(gcn::Graphics* graphics, const std::string& text, int x, int y, float r, float g, float b, float a);
		
	private:
		/**
		* Draws a single character of text
		*
		* @param Uint32 character A 32-bit character code
		**/
		void renderCharacter(Uint32 character, float &x, float &y);
};
