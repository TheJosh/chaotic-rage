// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;


#include "SPK_ColorQuadRenderer.h"
#include <Core/SPK_Particle.h>
#include <Core/SPK_Group.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../render_opengl/gl.h"


namespace SPK
{
namespace GL
{
	GL2ColorQuadRenderer::GL2ColorQuadRenderer(float size) :
		BaseQuadRenderer(),
		vao(0),
		vboBillboardVertex(0),
		vboPositions(0),
		vboColors(0),
		buffer(NULL),
		buffer_sz(0)
	{
		this->size[0] = this->size[1] = size;
	}

	GL2ColorQuadRenderer::~GL2ColorQuadRenderer()
	{
		free(buffer);
	}

	void GL2ColorQuadRenderer::initGLbuffers()
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// The VBO containing the 4 vertices of the particles.
		// Thanks to instancing, they will be shared by all particles.
		static const GLfloat quad_vertex_data[] = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			-0.5f, 0.5f, 0.0f,
			0.5f, 0.5f, 0.0f,
		};

		// Set up vertex buffer
		glGenBuffers(1, &vboBillboardVertex);
		glBindBuffer(GL_ARRAY_BUFFER, vboBillboardVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertex_data), quad_vertex_data, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribDivisor(0, 0);

		// Set up position buffer
		glGenBuffers(1, &vboPositions);
		glBindBuffer(GL_ARRAY_BUFFER, vboPositions);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribDivisor(1, 1);

		// Set up colors buffer
		glGenBuffers(1, &vboColors);
		glBindBuffer(GL_ARRAY_BUFFER, vboColors);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribDivisor(2, 1);

		glBindVertexArray(0);

		shaderIndex = createShaderProgram(
			"in vec3 vVertexPosition;\n"
			"in vec3 vParticlePosition;\n"
			"in vec4 vColor;\n"
			"out vec4 fColor;\n"
			"uniform vec3 camUp;\n"
			"uniform vec3 camRight;\n"
			"uniform vec2 size;\n"
			"uniform mat4 mVP;\n"
			
			"void main() {\n"
				"vec3 pos = vParticlePosition\n"
				"+ camRight * vVertexPosition.x * size.x\n"
				"+ camUp * vVertexPosition.y * size.y;\n"
				"gl_Position = mVP * vec4(pos, 1.0);\n"
				"fColor = vColor;\n"
			"}\n",

			"in vec4 fColor;\n"
			"void main() {\n"
				"gl_FragColor = fColor;\n"
			"}\n"
		);

		uniform_camUp = glGetUniformLocation(shaderIndex, "camUp");
		uniform_camRight = glGetUniformLocation(shaderIndex, "camRight");
		uniform_size = glGetUniformLocation(shaderIndex, "size");
		uniform_mvp = glGetUniformLocation(shaderIndex, "mVP");
	}

	void GL2ColorQuadRenderer::destroyGLbuffers()
	{
		glDeleteBuffers(1, &vboBillboardVertex);
		glDeleteBuffers(1, &vboPositions);
		glDeleteBuffers(1, &vboColors);
		glDeleteVertexArrays(1, &vao);
		glDeleteProgram(shaderIndex);

		vboBillboardVertex = 0;
		vboPositions = 0;
		vboColors = 0;
		vao = 0;
		shaderIndex = 0;
	}

	void GL2ColorQuadRenderer::render(const Group& group)
	{
		size_t num = group.getNbParticles();

		// Resize buffer if not large enough
		if (num > buffer_sz) {
			if (buffer_sz == 0) buffer_sz = 1024;
			while (buffer_sz < num) {
				buffer_sz *= 2;
			}
			free(buffer);
			buffer = (float*) malloc(sizeof(float) * 3 * buffer_sz);
		}

		// Copy data into buffer with the correct layout
		float* ptr = buffer;
		for (size_t i = 0; i < num; ++i)
		{
			const Particle& particle = group.getParticle(i);

			*ptr++ = particle.position().x;
			*ptr++ = particle.position().y;
			*ptr++ = particle.position().z;
		}

		// Set position data
		glBindBuffer(GL_ARRAY_BUFFER, vboPositions);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * num, buffer, GL_DYNAMIC_DRAW);

		// Set color data
		glBindBuffer(GL_ARRAY_BUFFER, vboColors);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * num, group.getParamAddress(PARAM_RED), GL_DYNAMIC_DRAW);

		glm::vec3 camUp = glm::vec3(v_matrix[0][1], v_matrix[1][1], v_matrix[2][1]);
		glm::vec3 camRight = glm::vec3(v_matrix[0][0], v_matrix[1][0], v_matrix[2][0]);

		// Bind VAO and shader, set uniforms, draw
		glBindVertexArray(vao);
		glUseProgram(shaderIndex);
		glUniform3fv(uniform_camUp, 1, glm::value_ptr(camUp));
		glUniform3fv(uniform_camRight, 1, glm::value_ptr(camRight));
		glUniform2fv(uniform_size, 1, this->size);
		glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(vp_matrix));
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, num);
		glBindVertexArray(0);
	}
}}
