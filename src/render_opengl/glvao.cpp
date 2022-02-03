// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <algorithm>
#include <map>
#include "gl.h"
#include "glvao.h"
#include "render_opengl.h"

using namespace std;



/**
* For VBO pointer offsets
**/
#define BUFFER_OFFSET(i) ((char *)NULL + (i))


/**
* Create a GLVAO object
**/
GLVAO::GLVAO()
{
	#ifdef OpenGL
		glGenVertexArrays(1, &this->vao);
		glBindVertexArray(this->vao);
	#endif

	this->index = 0;
	this->position = 0;
	this->color = 0;
	this->normal = 0;
	this->boneid = 0;
	this->boneweight = 0;
	this->tangent = 0;
	this->bitangent = 0;
	this->interleaved_pnt = 0;
	this->interleaved_pc = 0;
	this->interleaved_pc34 = 0;

	for (unsigned int i = 0; i < UV_CHANNELS; ++i) {
		this->texuv[i] = 0;
	}
}


/**
* Destroy the obj
**/
GLVAO::~GLVAO()
{
	glDeleteVertexArrays(1, &this->vao);
}


/**
* Index buffer
**/
void GLVAO::setIndex(GLuint vbo)
{
	this->index = vbo;
}


/**
* Position buffer. 3-coord
**/
void GLVAO::setPosition(GLuint vbo)
{
	glEnableVertexAttribArray(ATTRIB_POSITION);
	glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
	this->position = vbo;
}


/**
* Color buffer. 4-coord
**/
void GLVAO::setColor(GLuint vbo)
{
	glEnableVertexAttribArray(ATTRIB_COLOR);
	glVertexAttribPointer(ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, 0);
	this->color = vbo;
}


/**
* Normal buffer. 3-coord
**/
void GLVAO::setNormal(GLuint vbo)
{
	glEnableVertexAttribArray(ATTRIB_NORMAL);
	glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);
	this->normal = vbo;
}


/**
* Texture UVs. 3-coord
**/
void GLVAO::setTexUV(GLuint vbo, unsigned int channel)
{
	glEnableVertexAttribArray(ATTRIB_TEXUV + channel);
	glVertexAttribPointer(ATTRIB_TEXUV + channel, 3, GL_FLOAT, GL_FALSE, 0, 0);
	this->texuv[channel] = vbo;
}


/**
* Bone IDs. 4 x int
**/
void GLVAO::setBoneId(GLuint vbo)
{
	glEnableVertexAttribArray(ATTRIB_BONEID);
	glVertexAttribPointer(ATTRIB_BONEID, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0);
	this->boneid = vbo;
}


/**
* Bone weights. 4 x float
**/
void GLVAO::setBoneWeight(GLuint vbo)
{
	glEnableVertexAttribArray(ATTRIB_BONEWEIGHT);
	glVertexAttribPointer(ATTRIB_BONEWEIGHT, 4, GL_FLOAT, GL_FALSE, 0, 0);
	this->boneweight = vbo;
}


/**
* Tangents. 3-coord
**/
void GLVAO::setTangent(GLuint vbo)
{
	glEnableVertexAttribArray(ATTRIB_TANGENT);
	glVertexAttribPointer(ATTRIB_TANGENT, 3, GL_FLOAT, GL_FALSE, 0, 0);
	this->tangent = vbo;
}


/**
* Bitangents. 3-coord
**/
void GLVAO::setBitangent(GLuint vbo)
{
	glEnableVertexAttribArray(ATTRIB_BITANGENT);
	glVertexAttribPointer(ATTRIB_BITANGENT, 3, GL_FLOAT, GL_FALSE, 0, 0);
	this->bitangent = vbo;
}


/**
* Interleaved buffer, 3xPositions, 3xNormals, 2xTexUvs
**/
void GLVAO::setInterleavedPNT(GLuint vbo)
{
	glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(VBOvertex), BUFFER_OFFSET(0));   // Position
	glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(VBOvertex), BUFFER_OFFSET(12));    // Normals
	glVertexAttribPointer(ATTRIB_TEXUV, 2, GL_FLOAT, GL_FALSE, sizeof(VBOvertex), BUFFER_OFFSET(24));     // TexUVs

	glEnableVertexAttribArray(ATTRIB_POSITION);
	glEnableVertexAttribArray(ATTRIB_NORMAL);
	glEnableVertexAttribArray(ATTRIB_TEXUV);

	this->interleaved_pnt = vbo;
}


/**
* Interleaved buffer, 3xPositions, 3xColours
**/
void GLVAO::setInterleavedPC(GLuint vbo)
{
	glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 24, BUFFER_OFFSET(0));     // Position
	glVertexAttribPointer(ATTRIB_COLOR, 3, GL_FLOAT, GL_FALSE, 24, BUFFER_OFFSET(12));       // Colour

	glEnableVertexAttribArray(ATTRIB_POSITION);
	glEnableVertexAttribArray(ATTRIB_COLOR);

	this->interleaved_pc = vbo;
}


/**
* Interleaved buffer, 3xPositions, 4xColours
**/
void GLVAO::setInterleavedPC34(GLuint vbo)
{
	glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 28, BUFFER_OFFSET(0));     // Position
	glVertexAttribPointer(ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 28, BUFFER_OFFSET(12));       // Colour

	glEnableVertexAttribArray(ATTRIB_POSITION);
	glEnableVertexAttribArray(ATTRIB_COLOR);

	this->interleaved_pc34 = vbo;
}


/**
* Bind VAO for rendering
* If we are using software fallback, binds all the VBOs instead
**/
void GLVAO::bind()
{
	glBindVertexArray(this->vao);
}


/**
* When you are done loading or rendering
**/
void GLVAO::unbind()
{
	glBindVertexArray(0);
}
