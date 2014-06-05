// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "guichan_font.h"
#include "gl.h"
#include "gl_debug.h"
#include "../rage.h"
#include "../render/render_3d.h"
#include "../util/utf8.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


/**
* Gets the width of a string. The width of a string is not necesserily
* the sum of all the widths of it's glyphs.
*
* @param text The string to return the width of.
* @return The width of a string.
*/
int OpenGLFont::getWidth(const std::string& text) const
{
	return this->render->widthText(text);
}


/**
* Gets the height of the glyphs in the font.
*
* @return The height of the glyphs int the font.
*/
int OpenGLFont::getHeight() const
{
	return (int)round(this->height * 1.3f);
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
void OpenGLFont::drawString(gcn::Graphics* graphics, const std::string& text, int x, int y)
{
	if (this->render->face == NULL) return;
	if (this->render->font_vbo == 0) return;

	#ifdef OpenGL
		glBindVertexArray(0);
	#endif

	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	// Set up shader
	GLShader* shader = this->render->shaders["text"];
	glUseProgram(shader->p());
	glUniform1i(shader->uniform("uTex"), 0);
	glUniform4f(shader->uniform("uColor"), 0.0f, 0.0f, 0.0f, 1.0f);
	glUniformMatrix4fv(shader->uniform("uMVP"), 1, GL_FALSE, glm::value_ptr(this->render->ortho));

	// Determine starting X and Y coord
	const gcn::ClipRectangle& top = graphics->getCurrentClipArea();
	float xx = x + top.xOffset;
	float yy = y + top.yOffset + this->height;

	// Render each character
	const char* ptr = text.c_str();
	size_t textlen = strlen(ptr);
	while (textlen > 0) {
		Uint32 c = UTF8_getch(&ptr, &textlen);
		if (c == UNICODE_BOM_NATIVE || c == UNICODE_BOM_SWAPPED) {
			continue;
		}
		this->render->renderCharacter(c, xx, yy);
	}

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	CHECK_OPENGL_ERROR;
}

