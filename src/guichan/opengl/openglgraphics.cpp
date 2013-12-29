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

#include <string.h>

#include "../render_opengl/gl.h"
#include "../render_opengl/gl_debug.h"

#include "guichan/exception.hpp"
#include "guichan/image.hpp"
#include "guichan/opengl/openglimage.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace gcn
{
    class OpenGLGraphics_Impl {
        public:
            glm::mat4 projection;
            GLuint vbo;
            GLuint shader_image;
            GLuint shader_lines;

        public:
            OpenGLGraphics_Impl();

        public:
            void createShaders();
            GLuint createShaderProgram(const char *vs, const char *fs);
    };
    
    
    OpenGLGraphics::OpenGLGraphics()
    {
        setTargetPlane(640, 480);
        mAlpha = false;
        pimpl_ = new OpenGLGraphics_Impl();
    }

    OpenGLGraphics::OpenGLGraphics(int width, int height)
    {
        setTargetPlane(width, height);
        pimpl_ = new OpenGLGraphics_Impl();
    }

    OpenGLGraphics::~OpenGLGraphics()
    {
        if (pimpl_->vbo != 0)
        {
            glDeleteBuffers(1, &pimpl_->vbo);
        }
        
        delete(pimpl_);
    }

    OpenGLGraphics_Impl::OpenGLGraphics_Impl()
    {
        vbo = 0;
        shader_image = 0;
        shader_lines = 0;
    }
    
    GLuint OpenGLGraphics_Impl::createShaderProgram(const char *vs, const char *fs)
    {
        GLuint program, sVS, sFS;
        GLint len, success;

        // Create stuff
        program = glCreateProgram();
        assert(program);

        // Compile vertex shader
        sVS = glCreateShader(GL_VERTEX_SHADER);
        assert(sVS);
        len = strlen(vs);
        glShaderSource(sVS, 1, &vs, &len);
        glCompileShader(sVS);
        glAttachShader(program, sVS);

		// Check status
		glGetShaderiv(sVS, GL_COMPILE_STATUS, &success);
        if (! success) {
            GLchar infolog[1024];
            glGetShaderInfoLog(sVS, 1024, NULL, infolog);
            GL_LOG("Error compiling vertex shader:\n%s", infolog);
            assert(0);
        }

        // Compile fragment shader
        sFS = glCreateShader(GL_FRAGMENT_SHADER);
        assert(sFS);
        len = strlen(fs);
        glShaderSource(sFS, 1, &fs, &len);
        glCompileShader(sFS);
        glAttachShader(program, sFS);

		// Check status
		glGetShaderiv(sFS, GL_COMPILE_STATUS, &success);
        if (! success) {
            GLchar infolog[1024];
            glGetShaderInfoLog(sFS, 1024, NULL, infolog);
            GL_LOG("Error compiling fragment shader:\n%s", infolog);
            assert(0);
        }

        glBindAttribLocation(program, 0, "vPosition");
        glBindAttribLocation(program, 1, "vTexUV");

        // Link
        glLinkProgram(program);
        glDeleteShader(sVS);
        glDeleteShader(sFS);

        // Check for link errors
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (! success) {
            GLchar infolog[1024];
            glGetProgramInfoLog(program, 1024, NULL, infolog);
            GL_LOG("Error linking program:\n%s", infolog);
            assert(0);
        }

        return program;
    }

    void OpenGLGraphics_Impl::createShaders()
    {
        // Images
        shader_image = createShaderProgram(
            "attribute vec3 vPosition;"
            "attribute vec2 vTexUV;"
            "varying vec2 fTexUV;"
            "uniform mat4 uMVP;"
            "void main() {"
                "gl_Position = uMVP * vec4(vPosition, 1.0); fTexUV = vTexUV;"
            "}",

            "varying vec2 fTexUV;"
            "uniform sampler2D uTex;"
            "void main() {"
                "gl_FragColor = texture2D(uTex, fTexUV);"
            "}"
        );
 
        // Lines
        shader_lines = createShaderProgram(
            "attribute vec3 vPosition;"
            "uniform mat4 uMVP;"
            "void main() {"
                "gl_Position = uMVP * vec4(vPosition, 1.0);"
            "}",

            "uniform vec4 uColor;"
            "void main() {"
                "gl_FragColor = uColor;"
            "}"
        );
        
        CHECK_OPENGL_ERROR;
    }
    
    void OpenGLGraphics::_beginDraw()
    {
        if (pimpl_->shader_image == 0)
        {
             pimpl_->createShaders();

             pimpl_->projection = glm::ortho<float>(0.0f, mWidth, mHeight, 0.0f, -1.0f, 1.0f);

             glUseProgram(pimpl_->shader_image);
             glUniformMatrix4fv(glGetUniformLocation(pimpl_->shader_image, "uMVP"), 1, GL_FALSE, glm::value_ptr(pimpl_->projection));

             glUseProgram(pimpl_->shader_lines);
             glUniformMatrix4fv(glGetUniformLocation(pimpl_->shader_lines, "uMVP"), 1, GL_FALSE, glm::value_ptr(pimpl_->projection));
             glUniform4f(glGetUniformLocation(pimpl_->shader_lines, "uColor"), 0.0f, 0.0f, 0.0f, 0.0f);

             CHECK_OPENGL_ERROR;
        }

        if (pimpl_->vbo == 0)
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
        
        CHECK_OPENGL_ERROR;
    }

    void OpenGLGraphics::_endDraw()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);

        popClipArea();
        glDisable(GL_SCISSOR_TEST);
        
        CHECK_OPENGL_ERROR;
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
            {dstX, dstY + height, 0.0f, texX1, texY2},
            {dstX, dstY, 0.0f, texX1, texY1},
            {dstX + width, dstY + height, 0.0f, texX2, texY2},
            {dstX + width, dstY, 0.0f, texX2, texY1},
        };

        glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, 0);   // Position
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, ((char*)NULL + 12));     // TexUVs
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glUseProgram(pimpl_->shader_image);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Don't disable blending if the color has alpha
        if (!mAlpha)
        {
            glDisable(GL_BLEND);
        }
        
        CHECK_OPENGL_ERROR;
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

        glBufferData(GL_ARRAY_BUFFER, sizeof point, point, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        glUseProgram(pimpl_->shader_lines);
        glDrawArrays(GL_POINTS, 0, 1);
        
        CHECK_OPENGL_ERROR;
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

        GLfloat line[2][3] = {
            {x1 + 0.375f, y1 + 0.375f, 0.0f},
            {x2 + 1.0f - 0.375f, y2 + 1.0f - 0.375f, 0.0f}
        };

        glBufferData(GL_ARRAY_BUFFER, sizeof line, line, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        glUseProgram(pimpl_->shader_lines);
        glDrawArrays(GL_LINES, 0, 2);
        
        CHECK_OPENGL_ERROR;
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

        glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        glUseProgram(pimpl_->shader_lines);
        glDrawArrays(GL_LINE_LOOP, 0, 4);
        
        CHECK_OPENGL_ERROR;
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
            {rectangle.x + rectangle.width + top.xOffset, rectangle.y + top.yOffset, 0.0f},
            {rectangle.x + top.xOffset, rectangle.y + rectangle.height + top.yOffset, 0.0f},
            {rectangle.x + rectangle.width + top.xOffset, rectangle.y + rectangle.height + top.yOffset, 0.0f},
        };

        glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
        
        glUseProgram(pimpl_->shader_lines);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        
        CHECK_OPENGL_ERROR;
    }

    void OpenGLGraphics::setColor(const Color& color)
    {
        mColor = color;

        glUseProgram(pimpl_->shader_lines);
        glUniform4f(
            glGetUniformLocation(pimpl_->shader_lines, "uColor"),
            color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f
        );

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
