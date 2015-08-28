// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once


class RenderOpenGLSettings
{
	public:
		// OpenGL multisampled buffers (MSAA)
		// Values depend on video card;
		// Typical: 1, 2, 4, 8, 16
		int msaa;

		// Texture filtering
		// 1 = GL_NEAREST (worst), 4 = GL_LINEAR_MIPMAP_LINEAR (best)
		int tex_filter;

		// Anisotropy
		// Requires GL_EXT_texture_filter_anisotropic
		float tex_aniso;


	public:
		RenderOpenGLSettings():
			msaa(1),
			tex_filter(1),
			tex_aniso(4.0f)
			{}
};
