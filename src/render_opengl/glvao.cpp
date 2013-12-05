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
	#ifdef OpenGL
		glGenVertexArrays(1, &this->vao);
		glBindVertexArray(this->vao);
	#endif
	
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
	#ifdef OpenGL
		glDeleteVertexArrays(1, &this->vao);
	#endif
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
void GLVAO::setTexUV(GLuint vbo)
{
	glEnableVertexAttribArray(ATTRIB_TEXUV);
	glVertexAttribPointer(ATTRIB_TEXUV, 3, GL_FLOAT, GL_FALSE, 0, 0);
	this->texuv = vbo;
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
	#if defined(GLES)
		this->bindBuffers();
	#elif defined(OpenGL)
		glBindVertexArray(this->vao);
	#endif
}


/**
* When you are done loading or rendering
**/
void GLVAO::unbind()
{
	#if defined(GLES)
		this->unbindBuffers();
	#elif defined(OpenGL)
		glBindVertexArray(0);
	#endif
}


/**
* Manually bind all the buffers
*
* Internally, we use setPosition, setNormal, etc because those call glVertexAttribPointer
**/
void GLVAO::bindBuffers()
{
	// Index
	if (this->index) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->index);
	}
	
	// Position
	if (this->position) {
		glBindBuffer(GL_ARRAY_BUFFER, this->position);
		this->setPosition(this->position);
	}
	
	// Normals
	if (this->normal) {
		glBindBuffer(GL_ARRAY_BUFFER, this->normal);
		this->setNormal(this->normal);
	}
	
	// Texture uvs
	if (this->texuv) {
		glBindBuffer(GL_ARRAY_BUFFER, this->texuv);
		this->setTexUV(this->texuv);
	}
	
	// Bone id
	if (this->boneid) {
		glBindBuffer(GL_ARRAY_BUFFER, this->boneid);
		this->setBoneId(this->boneid);
	}
	
	// Bone weight
	if (this->boneweight) {
		glBindBuffer(GL_ARRAY_BUFFER, this->boneweight);
		this->setBoneWeight(this->boneweight);
	}
	
	// Tangent
	if (this->tangent) {
		glBindBuffer(GL_ARRAY_BUFFER, this->tangent);
		this->setTangent(this->tangent);
	}
	
	// Interleaved, position normal texture
	if (this->interleaved_pnt) {
		glBindBuffer(GL_ARRAY_BUFFER, this->interleaved_pnt);
		this->setInterleavedPNT(this->interleaved_pnt);
	}
	
	// Interleaved, position colour
	if (this->interleaved_pc) {
		glBindBuffer(GL_ARRAY_BUFFER, this->interleaved_pc);
		this->setInterleavedPC(this->interleaved_pc);
	}
}


/**
* Manually unbind the buffers
**/
void GLVAO::unbindBuffers()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


