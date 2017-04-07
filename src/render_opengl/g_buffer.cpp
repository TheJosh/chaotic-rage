// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "g_buffer.h"
#include "gl.h"
#include "gl_debug.h"


void GBuffer::init(unsigned int width, unsigned int height)
{
    glGenTextures(3, &this->tex_position);
    glGenRenderbuffers(1, &this->rbo_depth);
    glGenFramebuffers(1, &this->fbo);

    glBindTexture(GL_TEXTURE_2D, this->tex_position);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, this->tex_diffuse);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, this->tex_normal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    glBindRenderbuffer(GL_RENDERBUFFER, this->rbo_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->tex_position, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, this->tex_diffuse, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, this->tex_normal, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->rbo_depth);

    GLenum bufs[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, bufs);

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
    glDeleteRenderbuffers(1, &this->rbo_depth);
    glDeleteFramebuffers(1, &this->fbo);
}
