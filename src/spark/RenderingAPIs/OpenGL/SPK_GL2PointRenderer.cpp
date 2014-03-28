//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2009 - Julien Fryer - julienfryer@gmail.com				//
//																				//
// This software is provided 'as-is', without any express or implied			//
// warranty.  In no event will the authors be held liable for any damages		//
// arising from the use of this software.										//
//																				//
// Permission is granted to anyone to use this software for any purpose,		//
// including commercial applications, and to alter it and redistribute it		//
// freely, subject to the following restrictions:								//
//																				//
// 1. The origin of this software must not be misrepresented; you must not		//
//	claim that you wrote the original software. If you use this software		//
//	in a product, an acknowledgment in the product documentation would be		//
//	appreciated but is not required.											//
// 2. Altered source versions must be plainly marked as such, and must not be	//
//	misrepresented as being the original software.							//
// 3. This notice may not be removed or altered from any source distribution.	//
//////////////////////////////////////////////////////////////////////////////////


#include "RenderingAPIs/OpenGL/SPK_GL2PointRenderer.h"
#include "Core/SPK_Particle.h"
#include "Core/SPK_Group.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Used for the OpenGL debug fun
#include <stdio.h>

#define ATTRIB_POSITION 1
#define ATTRIB_COLOR 2

namespace SPK
{
namespace GL
{
	GL2PointRenderer::GL2PointRenderer(float size) :
		GLRenderer(),
		PointRendererInterface(POINT_SQUARE,size),
		textureIndex(0),
		worldSize(false),
		vaoIndex(0),
		vboPositionIndex(0),
		vboColorIndex(0)
	{}

	void GL2PointRenderer::initGLbuffers()
	{
		// Set up VAO
		glGenVertexArrays(1, &vaoIndex);
		glBindVertexArray(vaoIndex);

		// Set up position buffer
		glGenBuffers(1, &vboPositionIndex);
		glBindBuffer(GL_ARRAY_BUFFER, vboPositionIndex);
		glEnableVertexAttribArray(ATTRIB_POSITION);
		glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);

		// Set up colors buffer
		glGenBuffers(1, &vboColorIndex);
		glBindBuffer(GL_ARRAY_BUFFER, vboColorIndex);
		glEnableVertexAttribArray(ATTRIB_COLOR);
		glVertexAttribPointer(ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glBindVertexArray(0);

		// Create shader
		shaderIndex = createShaderProgram(
			"#version 140\n"
			"in vec3 vPosition;\n"
			"in vec4 vColor;\n"
			"out vec4 fColor;\n"
			"uniform mat4 uVP;\n"
			"void main() {\n"
				"gl_Position = uVP * vec4(vPosition, 1.0f);\n"
				"fColor = vColor;\n"
			"}\n",

			"#version 140\n"
			"in vec4 fColor;\n"
			"void main() {\n"
				"gl_FragColor = fColor;\n"
			"}\n"
		);

		shaderVPIndex = glGetUniformLocation(shaderIndex, "uVP");
	}

	void GL2PointRenderer::setVP(glm::mat4 vp)
	{
		vp_matrix = vp;
	}

	// TODO: This is cloned from Guichan; we should probably have a common function for this!
	GLuint GL2PointRenderer::createShaderProgram(const char *vs, const char *fs)
	{
		GLuint program, sVS, sFS;
		GLint len, success;

		// Create stuff
		program = glCreateProgram();

		// Compile vertex shader
		sVS = glCreateShader(GL_VERTEX_SHADER);
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
		}

		// Compile fragment shader
		sFS = glCreateShader(GL_FRAGMENT_SHADER);
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
		}

		glBindAttribLocation(program, ATTRIB_POSITION, "vPosition");
		glBindAttribLocation(program, ATTRIB_COLOR, "vColor");

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
		}

		CHECK_OPENGL_ERROR;

		return program;
	}

	void GL2PointRenderer::destroyGLbuffers()
	{
		glDeleteVertexArrays(1, &vaoIndex);
		glDeleteBuffers(1, &vboPositionIndex);
		glDeleteBuffers(1, &vboColorIndex);
		glDeleteProgram(shaderIndex);

		vaoIndex = 0;
		vboPositionIndex = 0;
		vboColorIndex = 0;
		shaderIndex = 0;
		shaderVPIndex = 0;
	}

	void GL2PointRenderer::render(const Group& group)
	{
		glEnable(GL_POINT_SMOOTH);
		glPointSize(2.0f);

		// Copy data into buffer with the correct layout
		// TODO: Reuse the buffer instead of malloc/free every frame
		size_t num = group.getNbParticles();
		float* data = (float*) malloc(sizeof(float) * 3 * num);
		float* ptr = data;
		for (size_t i = 0; i < num; ++i)
		{
			const Particle& particle = group.getParticle(i);

			*ptr++ = particle.position().x;
			*ptr++ = particle.position().y;
			*ptr++ = particle.position().z;
		}

		// Bind our VAO
		glBindVertexArray(vaoIndex);

		// Set position data
		glBindBuffer(GL_ARRAY_BUFFER, vboPositionIndex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * num, data, GL_DYNAMIC_DRAW);
		free(data);

		// Set color data
		glBindBuffer(GL_ARRAY_BUFFER, vboColorIndex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * num, group.getParamAddress(PARAM_RED), GL_DYNAMIC_DRAW);
		glVertexAttribPointer(ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, group.getParamStride(), 0);

		// Bind shader
		glUseProgram(shaderIndex);

		// Uniforms
		glUniformMatrix4fv(shaderVPIndex, 1, GL_FALSE, glm::value_ptr(vp_matrix));

		// Draw
		glDrawArrays(GL_POINTS, 0, num);

		// Clean up
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}}
