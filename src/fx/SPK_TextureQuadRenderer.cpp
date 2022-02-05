// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;


#include "SPK_TextureQuadRenderer.h"
#include <Core/SPK_Particle.h>
#include <Core/SPK_Group.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../render_opengl/texture_2d_array.h"
#include "../render_opengl/gl.h"


namespace SPK
{
namespace GL
{
	GL2InstanceQuadRenderer::GL2InstanceQuadRenderer(float size) :
		BaseQuadRenderer(),
		texture(NULL),
		vao(0),
		vboBillboardVertex(0),
		vboPositions(0),
		vboColors(0),
		buffer(NULL),
		buffer_sz(0)
	{
		this->size[0] = this->size[1] = size;
	}

	GL2InstanceQuadRenderer::~GL2InstanceQuadRenderer()
	{
		free(buffer);
	}

	void GL2InstanceQuadRenderer::initGLbuffers()
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
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
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
			"in vec4 vParticlePosition;\n"
			"in vec4 vColor;\n"
			"out vec4 fColor;\n"
			"out vec2 fTexUV;\n"
			"out float fTexIndex;\n"
			"uniform vec3 camUp;\n"
			"uniform vec3 camRight;\n"
			"uniform vec2 size;\n"
			"uniform mat4 mVP;\n"
			
			"void main() {\n"
				"vec3 pos = vParticlePosition.xyz\n"
				"+ camRight * vVertexPosition.x * size.x\n"
				"+ camUp * vVertexPosition.y * size.y;\n"
				"gl_Position = mVP * vec4(pos, 1.0);\n"
				"fColor = vColor;\n"
				"fTexUV = vVertexPosition.xy + vec2(0.5, 0.5);\n"
				"fTexIndex = vParticlePosition.w;\n"
			"}\n",

			"in vec4 fColor;\n"
			"in vec2 fTexUV;\n"
			"in float fTexIndex;\n"
			"uniform sampler2DArray tex;\n"
			"void main() {\n"
				"gl_FragColor = texture(tex, vec3(fTexUV.xy, fTexIndex)) * fColor;\n"
			"}\n"
		);

		uniform_camUp = glGetUniformLocation(shaderIndex, "camUp");
		uniform_camRight = glGetUniformLocation(shaderIndex, "camRight");
		uniform_size = glGetUniformLocation(shaderIndex, "size");
		uniform_mvp = glGetUniformLocation(shaderIndex, "mVP");
	}

	void GL2InstanceQuadRenderer::destroyGLbuffers()
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

	void GL2InstanceQuadRenderer::render(const Group& group)
	{
		float* ptr;
		size_t num = group.getNbParticles();

		// Unable to render if no texture set
		if (texture == NULL) {
			return;
		}

		// Resize buffer if not large enough
		if (num > buffer_sz) {
			if (buffer_sz == 0) buffer_sz = 1024;
			while (buffer_sz < num) {
				buffer_sz *= 2;
			}
			free(buffer);
			buffer = (float*) malloc(sizeof(float) * 4 * buffer_sz);
		}

		// Copy data into buffer with the correct layout
		// XYZ is coordinate and W is texture index
		ptr = buffer;
		for (size_t i = 0; i < num; ++i) {
			const Particle& particle = group.getParticle(i);
			*ptr++ = particle.position().x;
			*ptr++ = particle.position().y;
			*ptr++ = particle.position().z;
			*ptr++ = particle.getParamCurrentValue(SPK::PARAM_TEXTURE_INDEX);
		}

		// Set position data
		glBindBuffer(GL_ARRAY_BUFFER, vboPositions);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * num, buffer, GL_DYNAMIC_DRAW);

		// again for the color buffer
		ptr = buffer;
		for (size_t i = 0; i < num; ++i) {
			const Particle& particle = group.getParticle(i);
			*ptr++ = particle.getParamCurrentValue(SPK::PARAM_RED);
			*ptr++ = particle.getParamCurrentValue(SPK::PARAM_GREEN);
			*ptr++ = particle.getParamCurrentValue(SPK::PARAM_BLUE);
			*ptr++ = particle.getParamCurrentValue(SPK::PARAM_ALPHA);
		}

		// Set color data
		glBindBuffer(GL_ARRAY_BUFFER, vboColors);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * num, buffer, GL_DYNAMIC_DRAW);

		glm::vec3 camUp = glm::vec3(v_matrix[0][1], v_matrix[1][1], v_matrix[2][1]);
		glm::vec3 camRight = glm::vec3(v_matrix[0][0], v_matrix[1][0], v_matrix[2][0]);

		glBindTexture(GL_TEXTURE_2D_ARRAY, texture->getTexIndex());
		
		// Bind VAO and shader, set uniforms, draw
		glBindVertexArray(vao);
		glUseProgram(shaderIndex);
		glUniform3fv(uniform_camUp, 1, glm::value_ptr(camUp));
		glUniform3fv(uniform_camRight, 1, glm::value_ptr(camRight));
		glUniform2fv(uniform_size, 1, this->size);
		glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(vp_matrix));
		
		glDepthMask(GL_FALSE);
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, num);
		glDepthMask(GL_TRUE);
		glBindVertexArray(0);
	}
}}
