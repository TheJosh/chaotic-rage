// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;


#include "SPK_BaseQuadRenderer.h"
#include "../render_opengl/gl.h"


namespace SPK
{
namespace GL
{
	BaseQuadRenderer::BaseQuadRenderer() : GLRenderer()
	{
	}

	BaseQuadRenderer::~BaseQuadRenderer()
	{
	}

	void BaseQuadRenderer::setVP(glm::mat4 viewMatrix, glm::mat4 viewProjectionMatrix)
	{
		v_matrix = viewMatrix;
		vp_matrix = viewProjectionMatrix;
	}

	// TODO: This is cloned from Guichan; we should probably have a common function for this!
	GLuint BaseQuadRenderer::createShaderProgram(const char *vs, const char *fs)
	{
		GLuint program, sVS, sFS;
		GLint success;
		const char* strings[2];
		GLint lengths[2];

		// Different header based on GL variant
		strings[0] = "#version 130\n";
		lengths[0] = strlen(strings[0]);

		// Create stuff
		program = glCreateProgram();

		// Compile vertex shader
		sVS = glCreateShader(GL_VERTEX_SHADER);
		strings[1] = vs;
		lengths[1] = strlen(vs);
		glShaderSource(sVS, 2, strings, lengths);
		glCompileShader(sVS);
		glAttachShader(program, sVS);

		// Check status
		glGetShaderiv(sVS, GL_COMPILE_STATUS, &success);
		if (! success) {
			GLchar infolog[1024];
			glGetShaderInfoLog(sVS, 1024, NULL, infolog);
			assert(0);
		}

		// Compile fragment shader
		sFS = glCreateShader(GL_FRAGMENT_SHADER);
		strings[1] = fs;
		lengths[1] = strlen(fs);
		glShaderSource(sFS, 2, strings, lengths);
		glCompileShader(sFS);
		glAttachShader(program, sFS);

		// Check status
		glGetShaderiv(sFS, GL_COMPILE_STATUS, &success);
		if (! success) {
			GLchar infolog[1024];
			glGetShaderInfoLog(sFS, 1024, NULL, infolog);
			assert(0);
		}

		glBindAttribLocation(program, 0, "vVertexPosition");
		glBindAttribLocation(program, 1, "vParticlePosition");
		glBindAttribLocation(program, 2, "vColor");

		// Link
		glLinkProgram(program);
		glDeleteShader(sVS);
		glDeleteShader(sFS);

		// Check for link errors
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (! success) {
			GLchar infolog[1024];
			glGetProgramInfoLog(program, 1024, NULL, infolog);
			assert(0);
		}

		return program;
	}

}}
