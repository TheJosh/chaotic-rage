/*      _______   __   __   __   ______   __   __   _______   __   __
 *     / _____/\ / /\ / /\ / /\ / ____/\ / /\ / /\ / ___  /\ /  |\/ /\
 *    / /\____\// / // / // / // /\___\// /_// / // /\_/ / // , |/ / /
 *   / / /__   / / // / // / // / /    / ___  / // ___  / // /| ' / /
 *  / /_// /\ / /_// / // / // /_/_   / / // / // /\_/ / // / |  / /
 * /______/ //______/ //_/ //_____/\ /_/ //_/ //_/ //_/ //_/ /|_/ /
 * \______\/ \______\/ \_\/ \_____\/ \_\/ \_\/ \_\/ \_\/ \_\/ \_\/
 *
 * Copyright (c) 2004 - 2008 Olof Naessén and Per Larsson
 *
 *
 * Per Larsson a.k.a finalman
 * Olof Naessén a.k.a jansem/yakslem
 *
 * Visit: http://guichan.sourceforge.net
 *
 * License: (BSD)
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of Guichan nor the names of its contributors may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * For comments regarding functions please see the header file.
 */

#include "guichan/opengl/openglgraphics.hpp"

#if defined (_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "../render/gl.h"

#include "guichan/exception.hpp"
#include "guichan/image.hpp"
#include "guichan/opengl/openglimage.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace gcn
{
    class OpenGLGraphics_Impl {
        public:
            glm::mat4 projection;
            unsigned int vbo;
            GLuint shader_image;
            GLuint shader_lines;
    };
    
    
    OpenGLGraphics::OpenGLGraphics()
    {
        setTargetPlane(640, 480);
        mAlpha = false;
        pimpl_ = new OpenGLGraphics_Impl();
        createShaders();
    }

    OpenGLGraphics::OpenGLGraphics(int width, int height)
    {
        setTargetPlane(width, height);
        pimpl_ = new OpenGLGraphics_Impl();
        createShaders();
    }

    OpenGLGraphics::~OpenGLGraphics()
    {
        if (! pimpl_->vbo)
        {
            glDeleteBuffers(1, &pimpl_->vbo);
        }
        
        delete(pimpl_);
    }

    void OpenGLGraphics::createShaders()
    {
        pimpl_->shader_image = 0;
        pimpl_->shader_lines = 0;
        
        // TODO: Create some shaders
        
    }
    
    void OpenGLGraphics::_beginDraw()
    {
        pimpl_->projection = glm::ortho<float>(0.0f, mWidth, mHeight, 0.0f, -1.0f, 1.0f);

        if (! pimpl_->vbo)
        {
            glGenBuffers(1, &pimpl_->vbo);
        }

        glBindBuffer(GL_ARRAY_BUFFER, pimpl_->vbo);

        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_TEXTURE_2D);

        glLineWidth(1.0);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_SCISSOR_TEST);
        pushClipArea(Rectangle(0, 0, mWidth, mHeight));
    }

    void OpenGLGraphics::_endDraw()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);

        popClipArea();
        glDisable(GL_SCISSOR_TEST);
    }

    bool OpenGLGraphics::pushClipArea(Rectangle area)
    {
        bool result = Graphics::pushClipArea(area);

        glScissor(mClipStack.top().x,
                  mHeight - mClipStack.top().y - mClipStack.top().height,
                  mClipStack.top().width,
                  mClipStack.top().height);

        return result;
    }

    void OpenGLGraphics::popClipArea()
    {
        Graphics::popClipArea();

        if (mClipStack.empty())
        {
            return;
        }

        glScissor(mClipStack.top().x,
                  mHeight - mClipStack.top().y - mClipStack.top().height,
                  mClipStack.top().width,
                  mClipStack.top().height);
    }

    void OpenGLGraphics::setTargetPlane(int width, int height)
    {
        mWidth = width;
        mHeight = height;
    }

    void OpenGLGraphics::drawImage(const Image* image,
                                   int srcX,
                                   int srcY,
                                   int dstX,
                                   int dstY,
                                   int width,
                                   int height)
    {
		const OpenGLImage* srcImage = dynamic_cast<const OpenGLImage*>(image);

        if (srcImage == NULL)
        {
            throw GCN_EXCEPTION("Trying to draw an image of unknown format, must be an OpenGLImage.");
        }

        if (mClipStack.empty())
        {
            throw GCN_EXCEPTION("Clip stack is empty, perhaps you called a draw funtion outside of _beginDraw() and _endDraw()?");
        }

        const ClipRectangle& top = mClipStack.top();

        dstX += top.xOffset;
        dstY += top.yOffset;

        // Find OpenGL texture coordinates
        float texX1 = srcX / (float)srcImage->getTextureWidth();
        float texY1 = srcY / (float)srcImage->getTextureHeight();
        float texX2 = (srcX+width) / (float)srcImage->getTextureWidth();
        float texY2 = (srcY+height) / (float)srcImage->getTextureHeight();

        glBindTexture(GL_TEXTURE_2D, srcImage->getTextureHandle());

        glEnable(GL_TEXTURE_2D);

        // Check if blending already is enabled
        if (!mAlpha)
        {
            glEnable(GL_BLEND);
        }

        // Draw a textured quad -- the image
        GLfloat box[4][5] = {
            {dstX, dstY, 0.0f, texX1, texY1},
            {dstX, dstY + height, 0.0f, texX1, texY2},
            {dstX + width, dstY + height, 0.0f, texX2, texY2},
            {dstX + width, dstY, 0.0f, texX2, texY1},
        };

        glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, 0);   // Position
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, ((char*)NULL + 12));     // TexUVs
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(2);

        glUseProgram(pimpl_->shader_image);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Don't disable blending if the color has alpha
        if (!mAlpha)
        {
            glDisable(GL_BLEND);
        }
    }

    void OpenGLGraphics::drawPoint(int x, int y)
    {
        if (mClipStack.empty())
        {
            throw GCN_EXCEPTION("Clip stack is empty, perhaps you called a draw funtion outside of _beginDraw() and _endDraw()?");
        }

        const ClipRectangle& top = mClipStack.top();

        x += top.xOffset;
        y += top.yOffset;

        GLfloat point[3] = { x, y, 0.0f };

        glUseProgram(pimpl_->shader_lines);
        glBufferData(GL_ARRAY_BUFFER, sizeof point, point, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_POINTS, 0, 1);
    }

    void OpenGLGraphics::drawLine(int x1, int y1, int x2, int y2)
    {
        if (mClipStack.empty())
        {
            throw GCN_EXCEPTION("Clip stack is empty, perhaps you called a draw funtion outside of _beginDraw() and _endDraw()?");
        }

        const ClipRectangle& top = mClipStack.top();

        x1 += top.xOffset;
        y1 += top.yOffset;
        x2 += top.xOffset;
        y2 += top.yOffset;

        GLfloat line[4][3] = {
            {x1 + 0.375f, y1 + 0.375f, 0.0f},
            {x2 + 1.0f - 0.375f, y2 + 1.0f - 0.375f, 0.0f}
        };

        glUseProgram(pimpl_->shader_lines);
        glBufferData(GL_ARRAY_BUFFER, sizeof line, line, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_LINES, 0, 1);
    }

    void OpenGLGraphics::drawRectangle(const Rectangle& rectangle)
    {
        if (mClipStack.empty())
        {
            throw GCN_EXCEPTION("Clip stack is empty, perhaps you called a draw funtion outside of _beginDraw() and _endDraw()?");
        }

        const ClipRectangle& top = mClipStack.top();

        GLfloat box[4][3] = {
            {rectangle.x + top.xOffset, rectangle.y + top.yOffset, 0.0f},
            {rectangle.x + rectangle.width + top.xOffset - 1.0f, rectangle.y + top.yOffset + 0.375f, 0.0f},
            {rectangle.x + rectangle.width + top.xOffset - 1.0f, rectangle.y + rectangle.height + top.yOffset, 0.0f},
            {rectangle.x + top.xOffset, rectangle.y + rectangle.height + top.yOffset, 0.0f},
        };

        glUseProgram(pimpl_->shader_lines);
        glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_LINE_LOOP, 0, 4);
    }

    void OpenGLGraphics::fillRectangle(const Rectangle& rectangle)
    {
        if (mClipStack.empty())
        {
            throw GCN_EXCEPTION("Clip stack is empty, perhaps you called a draw funtion outside of _beginDraw() and _endDraw()?");
        }

        const ClipRectangle& top = mClipStack.top();

        GLfloat box[4][3] = {
            {rectangle.x + top.xOffset, rectangle.y + top.yOffset, 0.0f},
            {rectangle.x + rectangle.width + top.xOffset - 1.0f, rectangle.y + top.yOffset + 0.375f, 0.0f},
            {rectangle.x + rectangle.width + top.xOffset - 1.0f, rectangle.y + rectangle.height + top.yOffset, 0.0f},
            {rectangle.x + top.xOffset, rectangle.y + rectangle.height + top.yOffset, 0.0f},
        };

        glUseProgram(pimpl_->shader_lines);
        glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void OpenGLGraphics::setColor(const Color& color)
    {
        mColor = color;
        /*glColor4ub((GLubyte) color.r,
                   (GLubyte) color.g,
                   (GLubyte) color.b,
                   (GLubyte) color.a);*/

        mAlpha = color.a != 255;

        if (mAlpha)
        {
            glEnable(GL_BLEND);
        }
    }

    const Color& OpenGLGraphics::getColor() const
    {
        return mColor;
    }
}
