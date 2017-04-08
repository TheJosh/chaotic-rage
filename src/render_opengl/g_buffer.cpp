// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "g_buffer.h"
#include "gl.h"
#include "gl_debug.h"


void GBuffer::init(unsigned int width, unsigned int height)
{
    glGenTextures(3, &this->tex_diffuse);
    glGenFramebuffers(1, &this->fbo);

    glBindTexture(GL_TEXTURE_2D, this->tex_diffuse);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, this->tex_normal);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, this->tex_depth);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->tex_diffuse, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, this->tex_normal, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->tex_depth, 0);

    GLenum bufs[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, bufs);

    GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        switch (status) {
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: GL_LOG("Framebuffer status %i GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT", status); break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: GL_LOG("Framebuffer status %i GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT", status); break;
            case GL_FRAMEBUFFER_UNSUPPORTED: GL_LOG("Framebuffer status %i GL_FRAMEBUFFER_UNSUPPORTED", status); break;
            default: GL_LOG("Framebuffer status %i unknown", status); break;
        }
        reportFatalError("Error creating g-buffer");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    CHECK_OPENGL_ERROR
}


void GBuffer::cleanup()
{
    glDeleteTextures(3, &this->tex_diffuse);
    glDeleteFramebuffers(1, &this->fbo);
}
