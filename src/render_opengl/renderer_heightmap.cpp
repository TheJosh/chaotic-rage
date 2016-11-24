// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;


#include "renderer_heightmap.h"
#include "glshader.h"
#include "render_opengl.h"
#include "glvao.h"
#include "gl_debug.h"
#include "../map/heightmap.h"
#include "../game_state.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <sstream>

extern glm::mat4 biasMatrix;

using namespace std;


/**
* Prepare the heightmap renderer
**/
RendererHeightmap::RendererHeightmap(RenderOpenGL* render, Heightmap* heightmap)
	: heightmap(heightmap)
{
	this->shader = this->createShader(render, heightmap);
	this->createVAO();
}


RendererHeightmap::~RendererHeightmap()
{
	delete this->vao;
	delete this->shader;
}


/**
* Create a shader, optimised for the specific splat rendering used
**/
GLShader* RendererHeightmap::createShader(RenderOpenGL* render, Heightmap* heightmap)
{
	CHECK_OPENGL_ERROR;

	GLShader* s = new GLShader(0);
	
	char* vertex_code = s->loadCodeFile("phong_static.glslv");
	
	GLuint vertex = s->createShader(GL_VERTEX_SHADER, vertex_code);
	free(vertex_code);
	if (vertex == 0) {
		GL_LOG("Invalid vertex shader");
		return NULL;
	}

	char* base_fragment_code = s->loadCodeFile("phong_splat.glslf");
	char* diffuse_code = RendererHeightmap::createShaderFunction_diffuseColor(heightmap);

	const char* strings[3];
	strings[0] = "#version 130\n";
	strings[1] = base_fragment_code;
	strings[2] = diffuse_code;

	GLuint fragment = s->createShader(GL_FRAGMENT_SHADER, 3, strings);
	free(base_fragment_code);
	free(diffuse_code);
	if (fragment == 0) {
		glDeleteShader(vertex);
		GL_LOG("Invalid fragment shader");
		return NULL;
	}

	bool result = s->createProgFromShaders(vertex, fragment);

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	if (!result) {
		return NULL;
	}

	glUseProgram(s->p());
	glUniform1i(s->uniform("uTex"), 0);
	glUniform1i(s->uniform("uShadowMap"), 1);
	glUniform1i(s->uniform("uNormal"), 2);
	glUniform1i(s->uniform("uLightmap"), 3);
	glUniform1i(s->uniform("uDayNight"), 4);
	glUniform1i(s->uniform("uAlphaMap"), 0);
	glUniform1i(s->uniform("uLayers[0]"), 5);
	glUniform1i(s->uniform("uLayers[1]"), 6);
	glUniform1i(s->uniform("uLayers[2]"), 7);
	glUniform1i(s->uniform("uLayers[3]"), 8);
	glUniform1i(s->uniform("uLightmap"), 9);
	glUniform1i(s->uniform("uDetail[0]"), 10);
	glUniform1i(s->uniform("uDetail[1]"), 11);
	glUniform1i(s->uniform("uDetail[2]"), 12);
	glUniform1i(s->uniform("uDetail[3]"), 13);

	CHECK_OPENGL_ERROR;

	return s;
}


/**
* Create the 'diffuseColor' method used in splat fragment shader
**/
char* RendererHeightmap::createShaderFunction_diffuseColor(Heightmap* heightmap)
{
	std::stringstream ss;

	ss << "vec4 diffuseColor() {\n";
	ss << "vec4 diffuse = vec4(0.0, 0.0, 0.0, 0.0);\n";

	if (debug_enabled("terrain")) {
		// Dump tex coords
		ss << "diffuse += vec4(TexUV0, 0.0, 0.0);\n";
		
		// Also create a checkerboard pattern
		ss << "float cellX = TexUV0.x * " << heightmap->getDataSizeX() << ";\n";
		ss << "float cellZ = TexUV0.y * " << heightmap->getDataSizeZ() << ";\n";
		ss << "if ((mod(cellX, 2.0) < 1.0 && mod(cellZ, 2.0) > 1.0) || (mod(cellX, 2.0) > 1.0 && mod(cellZ, 2.0) < 1.0)) {\n";
		ss << "    diffuse *= 0.5;\n";
		ss << "}\n";
		
	} else if (heightmap->getBigTexture() != NULL) {
		ss << "diffuse += texture2D(uTex, TexUV0);";
	} else {
		ss << "vec4 alphaMap = texture2D(uAlphaMap, TexUV0);\n";
		for (unsigned int i = 0; i < TEXTURE_SPLAT_LAYERS; ++i) {
			TextureSplatLayer *lyr = &heightmap->layers[i];

			if (lyr->texture != NULL) {
				ss << "diffuse += ";
				ss << "alphaMap[" << i << "] * texture2D(uLayers[" << i << "], TexUV0 * " << lyr->scale << ")";
				if (lyr->dbl) {
					ss << "* texture2D(uLayers[" << i << "], TexUV0 * " << (lyr->scale * -0.25f) << ") * 1.5";
				}
				if (lyr->detail != NULL) {
					ss << "* texture2D(uDetail[" << i << "], TexUV0 * " << (lyr->scale * lyr->detail_scale) << ") * 1.5";
				}
				ss << ";\n";
			}
		}
	}

	if (heightmap->getLightmap() != NULL) {
		ss << "diffuse *= texture2D(uLightmap, TexUV0);\n";
	}

	ss << "return diffuse;\n";
	ss << "}";

	// TODO: I'm sure there are better ways to do this!
	const std::string tmp = ss.str();
	const char* cstr = tmp.c_str();
	char* ret = (char*) malloc(strlen(cstr) + 1);
	memcpy(ret, cstr, strlen(cstr) + 1);
	return ret;
}


/**
* Create an OpenGL mesh (array of triangle strips) from a heightmap
**/
void RendererHeightmap::createVAO()
{
	unsigned int nX, nZ, j;
	float flX, flZ;
	GLuint buffer;

	unsigned int maxX = heightmap->getDataSizeX();
	unsigned int maxZ = heightmap->getDataSizeZ();

	unsigned int num_verts = maxX * maxZ;
	VBOvertex* vertexes = new VBOvertex[num_verts];

	// Create vertextes
	j = 0;
	for (nZ = 0; nZ < maxZ; nZ++) {
		for(nX = 0; nX < maxX; nX++) {

			/*// u = p2 - p1; v = p3 - p1
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
			);*/

			flX = static_cast<float>(nX);
			flZ = static_cast<float>(nZ);
			vertexes[j].x = flX;
			vertexes[j].y = heightmap->getValue(nX, nZ);
			vertexes[j].z = flZ;
			vertexes[j].nx = 0.0f;
			vertexes[j].ny = -1.0f;
			vertexes[j].nz = 0.0f;
			vertexes[j].tx = flX / heightmap->getDataSizeX();
			vertexes[j].ty = flZ / heightmap->getDataSizeZ();
			j++;
		}
	}
	assert(j == num_verts);

	// There is one fewer face then there are vertexes on each axis
	maxX--;
	maxZ--;

	unsigned int num_index = maxX * maxZ * 6;
	unsigned int stride = maxX + 1;
	Uint32* index = new Uint32[num_index];

	// Create indexes
	j = 0;
	for (nZ = 0; nZ < maxZ; nZ++) {
		for (nX = 0; nX < maxX; nX++) {
			unsigned int top_left = nZ * (maxZ + 1) + nX;

			// TL, BL, TR
			index[j++] = top_left;
			index[j++] = top_left + stride;
			index[j++] = top_left + 1;

			// BL, BR, TR
			index[j++] = top_left + stride;
			index[j++] = top_left + stride + 1;
			index[j++] = top_left + 1;
		}
	}
	assert(j == num_index);

	// Create VAO
	this->vao = new GLVAO();

	// Create interleaved VBO
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VBOvertex) * num_verts, vertexes, GL_STATIC_DRAW);
	this->vao->setInterleavedPNT(buffer);

	// Create index buffer
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Uint32) * num_index, index, GL_STATIC_DRAW);
	this->vao->setIndex(buffer);

	delete [] vertexes;
	delete [] index;
}


/**
* Bind the 'splat' tectures
**/
void RendererHeightmap::bindSplatTextures()
{
	for (unsigned int i = 0; i < TEXTURE_SPLAT_LAYERS; ++i) {
		if (heightmap->layers[i].texture == NULL) break;
		glActiveTexture(GL_TEXTURE5 + i);
		glBindTexture(GL_TEXTURE_2D, heightmap->layers[i].texture->pixels);
	}

	for (unsigned int i = 0; i < TEXTURE_SPLAT_LAYERS; ++i) {
		if (heightmap->layers[i].detail == NULL) break;
		glActiveTexture(GL_TEXTURE10 + i);
		glBindTexture(GL_TEXTURE_2D, heightmap->layers[i].detail->pixels);
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightmap->alphamap->pixels);
}


/**
* Draw a heightmap
**/
void RendererHeightmap::draw(RenderOpenGL* render)
{
	if (heightmap->getBigNormal() != NULL) {
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, heightmap->getBigNormal()->pixels);
	}

	if (heightmap->getLightmap() != NULL) {
		glActiveTexture(GL_TEXTURE9);
		glBindTexture(GL_TEXTURE_2D, heightmap->getLightmap()->pixels);
	}

	glUseProgram(this->shader->p());

	if (heightmap->getBigTexture() != NULL) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, heightmap->getBigTexture()->pixels);
	} else {
		this->bindSplatTextures();
	}

	glm::mat4 modelMatrix = glm::scale(
		glm::mat4(1.0f),
		glm::vec3(heightmap->getScaleX(), 1.0f, heightmap->getScaleZ())
	);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-heightmap->getSizeX()/2.0f, 0.0f, -heightmap->getSizeZ()/2.0f));
	modelMatrix = glm::translate(modelMatrix, heightmap->getPosition());

	glm::mat4 MVP = render->projection * render->view * modelMatrix;
	glm::mat4 depthBiasMVP = biasMatrix * render->depthmvp * modelMatrix;
	
	glUniform1f(this->shader->uniform("uTimeOfDay"), render->st->time_of_day);
	glUniformMatrix4fv(this->shader->uniform("uMVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	glUniformMatrix4fv(this->shader->uniform("uM"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix3fv(this->shader->uniform("uMN"), 1, GL_FALSE, glm::value_ptr(glm::inverseTranspose(glm::mat3(modelMatrix))));
	glUniformMatrix4fv(this->shader->uniform("uV"), 1, GL_FALSE, glm::value_ptr(render->view));
	glUniformMatrix4fv(this->shader->uniform("uDepthBiasMVP"), 1, GL_FALSE, glm::value_ptr(depthBiasMVP));

	this->vao->bind();
	
	int numTiles = (heightmap->getDataSizeX() - 1) * (heightmap->getDataSizeZ() - 1);
	glDrawElements(GL_TRIANGLES, numTiles * 6, GL_UNSIGNED_INT, 0);
}
