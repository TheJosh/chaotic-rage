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


#include "RenderingAPIs/OpenGL/SPK_GL2LineRenderer.h"
#include "Core/SPK_Particle.h"
#include "Core/SPK_Group.h"
#include "../../../render_opengl/glvao.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Used for the OpenGL debug fun
#include <stdio.h>


namespace SPK
{
namespace GL
{
	GL2LineRenderer::GL2LineRenderer(float length,float width) :
		GLRenderer(),
		LineRendererInterface(length, width),
		vao(NULL),
		vboPositionColorIndex(0),
		buffer(NULL),
		buffer_sz(0)
	{}

	GL2LineRenderer::~GL2LineRenderer()
	{
		free(buffer);
	}

	void GL2LineRenderer::initGLbuffers()
	{
		vao = new GLVAO();

		// Set up position buffer
		glGenBuffers(1, &vboPositionColorIndex);
		glBindBuffer(GL_ARRAY_BUFFER, vboPositionColorIndex);
		vao->setInterleavedPC34(vboPositionColorIndex);

		// Create shader
		shaderIndex = createShaderProgram(
			"attribute vec3 vPosition;\n"
			"attribute vec4 vColor;\n"
			"varying vec4 fColor;\n"
			"uniform mat4 uVP;\n"
			"void main() {\n"
				"gl_Position = uVP * vec4(vPosition, 1.0);\n"
				"fColor = vColor;\n"
			"}\n",

			"varying vec4 fColor;\n"
			"void main() {\n"
				"gl_FragColor = fColor;\n"
			"}\n"
		);

		shaderVPIndex = glGetUniformLocation(shaderIndex, "uVP");
	}

	void GL2LineRenderer::setVP(glm::mat4 viewMatrix, glm::mat4 viewProjectionMatrix)
	{
		vp_matrix = viewProjectionMatrix;
	}

	// TODO: This is cloned from Guichan; we should probably have a common function for this!
	GLuint GL2LineRenderer::createShaderProgram(const char *vs, const char *fs)
	{
		GLuint program, sVS, sFS;
		GLint success;
		const char* strings[2];
		GLint lengths[2];

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
			GL_LOG("Error compiling vertex shader:\n%s", infolog);
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

	void GL2LineRenderer::destroyGLbuffers()
	{
		delete vao;
		glDeleteBuffers(1, &vboPositionColorIndex);
		glDeleteProgram(shaderIndex);

		vboPositionColorIndex = 0;
		shaderIndex = 0;
		shaderVPIndex = 0;
	}

	void GL2LineRenderer::render(const Group& group)
	{
		size_t num = group.getNbParticles();

		glEnable(GL_LINE_SMOOTH);

		// Resize buffer if not large enough
		if (num > buffer_sz) {
			if (buffer_sz == 0) buffer_sz = 1024;
			while (buffer_sz < num) {
				buffer_sz *= 2;
			}
			free(buffer);
			buffer = (float*) malloc(sizeof(float) * 7 * buffer_sz * 2);
		}

		// Copy data into buffer with the correct layout
		float* ptr = buffer;
		for (size_t i = 0; i < num; ++i)
		{
			const Particle& particle = group.getParticle(i);

			*ptr++ = particle.position().x;
			*ptr++ = particle.position().y;
			*ptr++ = particle.position().z;
			
			*ptr++ = particle.getR();
			*ptr++ = particle.getG();
			*ptr++ = particle.getB();
			*ptr++ = particle.getParamCurrentValue(PARAM_ALPHA);
			
			*ptr++ = particle.position().x + particle.velocity().x * length;
			*ptr++ = particle.position().y + particle.velocity().y * length;
			*ptr++ = particle.position().z + particle.velocity().z * length;
			
			*ptr++ = particle.getR();
			*ptr++ = particle.getG();
			*ptr++ = particle.getB();
			*ptr++ = particle.getParamCurrentValue(PARAM_ALPHA);
		}

		// Set position data
		glBindBuffer(GL_ARRAY_BUFFER, vboPositionColorIndex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 7 * num * 2, buffer, GL_DYNAMIC_DRAW);

		// Bind VAO and shader, set uniforms, draw
		vao->bind();
		glUseProgram(shaderIndex);
		glUniformMatrix4fv(shaderVPIndex, 1, GL_FALSE, glm::value_ptr(vp_matrix));
		glDrawArrays(GL_LINES, 0, num * 2);
		vao->unbind();
	}
}}
