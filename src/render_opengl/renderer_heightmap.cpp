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

	this->createVAO();
}


RendererHeightmap::~RendererHeightmap()
{
	delete this->vao;
}


/**
* Create an OpenGL mesh (array of triangle strips) from a heightmap
**/
void RendererHeightmap::createVAO()
{
	unsigned int nX, nZ, j;
	float flX, flZ;
	GLuint buffer;

	unsigned int maxX = heightmap->getDataSizeX() - 1;
	unsigned int maxZ = heightmap->getDataSizeZ() - 1;

	unsigned int glsize = (maxX * maxZ * 2) + (maxZ * 2);

	VBOvertex* vertexes = new VBOvertex[glsize];

	j = 0;
	for (nZ = 0; nZ < maxZ; nZ++) {
		for(nX = 0; nX < maxX; nX++) {

			// u = p2 - p1; v = p3 - p1
			btVector3 u =
				btVector3(static_cast<float>(nX) + 1.0f, heightmap->getValue(nX + 1, nZ + 1), static_cast<float>(nZ) + 1.0f) -
				btVector3(static_cast<float>(nX), heightmap->getValue(nX, nZ), static_cast<float>(nZ));
			btVector3 v =
				btVector3(static_cast<float>(nX) + 1.0f, heightmap->getValue(nX + 1, nZ), static_cast<float>(nZ)) -
				btVector3(static_cast<float>(nX), heightmap->getValue(nX, nZ), static_cast<float>(nZ));

			// calc vector
			btVector3 normal = btVector3(
				u.y() * v.z() - u.z() * v.y(),
				u.z() * v.x() - u.x() * v.z(),
				u.x() * v.y() - u.y() * v.x()
			);

			// First cell on the row has two extra verticies
			if (nX == 0) {
				flX = static_cast<float>(nX);
				flZ = static_cast<float>(nZ);
				vertexes[j].x = flX;
				vertexes[j].y = heightmap->getValue(nX, nZ);
				vertexes[j].z = flZ;
				vertexes[j].nx = normal.x();
				vertexes[j].ny = normal.y();
				vertexes[j].nz = normal.z();
				vertexes[j].tx = flX / heightmap->getDataSizeX();
				vertexes[j].ty = flZ / heightmap->getDataSizeZ();
				j++;

				flX = static_cast<float>(nX);
				flZ = static_cast<float>(nZ) + 1.0f;
				vertexes[j].x = flX;
				vertexes[j].y = heightmap->getValue(nX, nZ + 1);
				vertexes[j].z = flZ;
				vertexes[j].nx = normal.x();
				vertexes[j].ny = normal.y();
				vertexes[j].nz = normal.z();
				vertexes[j].tx = flX / heightmap->getDataSizeX();
				vertexes[j].ty = flZ / heightmap->getDataSizeZ();
				j++;
			}

			// Top
			flX = static_cast<float>(nX) + 1.0f;
			flZ = static_cast<float>(nZ);
			vertexes[j].x = flX;
			vertexes[j].y = heightmap->getValue(nX + 1, nZ);
			vertexes[j].z = flZ;
			vertexes[j].nx = normal.x();
			vertexes[j].ny = normal.y();
			vertexes[j].nz = normal.z();
			vertexes[j].tx = flX / heightmap->getDataSizeX();
			vertexes[j].ty = flZ / heightmap->getDataSizeZ();
			j++;

			// Bottom
			flX = static_cast<float>(nX) + 1.0f;
			flZ = static_cast<float>(nZ) + 1.0f;
			vertexes[j].x = flX;
			vertexes[j].y = heightmap->getValue(nX + 1, nZ + 1);
			vertexes[j].z = flZ;
			vertexes[j].nx = normal.x();
			vertexes[j].ny = normal.y();
			vertexes[j].nz = normal.z();
			vertexes[j].tx = flX / heightmap->getDataSizeX();
			vertexes[j].ty = flZ / heightmap->getDataSizeZ();
			j++;
		}
	}

	assert(j == glsize);

	// Create VAO
	this->vao = new GLVAO();

	// Create interleaved VBO
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VBOvertex) * glsize, vertexes, GL_STATIC_DRAW);
	this->vao->setInterleavedPNT(buffer);

	delete [] vertexes;
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

	this->vao->bind();
	
	int numPerStrip = 2 + ((heightmap->getDataSizeX()-1) * 2);
	for (int z = 0; z < heightmap->getDataSizeZ() - 1; z++) {
		glDrawArrays(GL_TRIANGLE_STRIP, numPerStrip * z, numPerStrip);
	}
}
