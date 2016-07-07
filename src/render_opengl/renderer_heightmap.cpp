// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;


#include "renderer_heightmap.h"
#include "glshader.h"
#include "render_opengl.h"
#include "glvao.h"
#include "../map/heightmap.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

extern glm::mat4 biasMatrix;


RendererHeightmap::RendererHeightmap(RenderOpenGL* render, Heightmap* heightmap)
	: heightmap(heightmap)
{
	if (heightmap->getBigTexture() == NULL) {
		this->shader = render->shaders[SHADER_TERRAIN_SPLAT];
	} else if (heightmap->getBigNormal() == NULL) {
		this->shader = render->shaders[SHADER_TERRAIN_PLAIN];
	} else {
		this->shader = render->shaders[SHADER_TERRAIN_NORMALMAP];
	}
}


RendererHeightmap::~RendererHeightmap()
{
}


void RendererHeightmap::draw(RenderOpenGL* render)
{
	if (heightmap->getBigNormal() != NULL) {
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, heightmap->getBigNormal()->pixels);
		glActiveTexture(GL_TEXTURE0);
	}

	glUseProgram(this->shader->p());

	if (heightmap->getBigTexture() != NULL) {
		glBindTexture(GL_TEXTURE_2D, heightmap->getBigTexture()->pixels);
	} else {
		heightmap->getSplatTexture()->bindTextures();
		heightmap->getSplatTexture()->setUniforms(this->shader);
	}

	glm::mat4 modelMatrix = glm::scale(
		glm::mat4(1.0f),
		glm::vec3(heightmap->getScaleX(), 1.0f, heightmap->getScaleZ())
	);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-heightmap->getSizeX()/2.0f, 0.0f, -heightmap->getSizeZ()/2.0f));
	modelMatrix = glm::translate(modelMatrix, heightmap->getPosition());

	glm::mat4 MVP = render->projection * render->view * modelMatrix;
	glm::mat4 depthBiasMVP = biasMatrix * render->depthmvp * modelMatrix;
	
	glUniformMatrix4fv(this->shader->uniform("uMVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	glUniformMatrix4fv(this->shader->uniform("uM"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix3fv(this->shader->uniform("uMN"), 1, GL_FALSE, glm::value_ptr(glm::inverseTranspose(glm::mat3(modelMatrix))));
	glUniformMatrix4fv(this->shader->uniform("uV"), 1, GL_FALSE, glm::value_ptr(render->view));
	glUniformMatrix4fv(this->shader->uniform("uDepthBiasMVP"), 1, GL_FALSE, glm::value_ptr(depthBiasMVP));

	this->heightmap->glvao->bind();
	
	int numPerStrip = 2 + ((heightmap->getDataSizeX()-1) * 2);
	for (int z = 0; z < heightmap->getDataSizeZ() - 1; z++) {
		glDrawArrays(GL_TRIANGLE_STRIP, numPerStrip * z, numPerStrip);
	}
}
