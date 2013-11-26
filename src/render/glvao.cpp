// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <algorithm>
#include <map>
#include "gl.h"
#include "../rage.h"
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
	glGenVertexArrays(1, &this->vao);
	glBindVertexArray(this->vao);

	this->index = 0;
	this->position = 0;
	this->normal = 0;
	this->texuv = 0;
	this->boneid = 0;
	this->boneweight = 0;
	this->tangent = 0;
	this->interleaved_pnt = 0;
	this->interleaved_pc = 0;
}


/**
* Destroy the obj
**/
GLVAO::~GLVAO()
{
	// TODO: kill buffer
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
	glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, 0, 0, 0);
	this->position = vbo;
}


/**
* Normal buffer. 3-coord
**/
void GLVAO::setNormal(GLuint vbo)
{
	glEnableVertexAttribArray(ATTRIB_NORMAL);
	glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, 0, 0, 0);
	this->normal = vbo;
}


/**
* Texture UVs. 3-coord
**/
void GLVAO::setTexUV(GLuint vbo)
{
	glEnableVertexAttribArray(ATTRIB_TEXUV);
	glVertexAttribPointer(ATTRIB_TEXUV, 3, GL_FLOAT, 0, 0, 0);
	this->texuv = vbo;
}


/**
* Bone IDs. 4 x int
**/
void GLVAO::setBoneId(GLuint vbo)
{
	glEnableVertexAttribArray(ATTRIB_BONEID);
	glVertexAttribPointer(ATTRIB_BONEID, 4, GL_INT, 0, 0, 0);
	this->boneid = vbo;
}


/**
* Bone weights. 4 x float
**/
void GLVAO::setBoneWeight(GLuint vbo)
{
	glEnableVertexAttribArray(ATTRIB_BONEWEIGHT);
	glVertexAttribPointer(ATTRIB_BONEWEIGHT, 4, GL_FLOAT, 0, 0, 0);
	this->boneweight = vbo;
}


/**
* Tangents. 3-coord
**/
void GLVAO::setTangent(GLuint vbo)
{
	glEnableVertexAttribArray(ATTRIB_TANGENT);
	glVertexAttribPointer(ATTRIB_TANGENT, 3, GL_FLOAT, 0, 0, 0);
	this->tangent = vbo;
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


