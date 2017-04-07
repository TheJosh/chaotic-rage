// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "gl.h"


class GBuffer
{
    public:
        void init(unsigned int width, unsigned int height);
        void cleanup();

    public:
        GLuint tex_position;
        GLuint tex_diffuse;
        GLuint tex_normal;
        GLuint rbo_depth;
        GLuint fbo;
};
