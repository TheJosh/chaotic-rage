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
}


void GLVAO::setIndex(GLuint vbo)
{
	this->index = vbo;
}

void GLVAO::setPosition(GLuint vbo)
{
	glEnableVertexAttribArray(ATTRIB_POSITION);
	glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, 0, 0, 0);
	this->position = vbo;
}

void GLVAO::setNormal(GLuint vbo)
{
	glEnableVertexAttribArray(ATTRIB_NORMAL);
	glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, 0, 0, 0);
	this->normal = vbo;
}

void GLVAO::setTexUV(GLuint vbo)
{
	glEnableVertexAttribArray(ATTRIB_TEXUV);
	glVertexAttribPointer(ATTRIB_TEXUV, 3, GL_FLOAT, 0, 0, 0);
	this->texuv = vbo;
}

void GLVAO::setBoneId(GLuint vbo)	
{
	glEnableVertexAttribArray(ATTRIB_BONEID);
	glVertexAttribPointer(ATTRIB_BONEID, 4, GL_INT, 0, 0, 0);
	this->boneid = vbo;
}

void GLVAO::setBoneWeight(GLuint vbo)
{
	glEnableVertexAttribArray(ATTRIB_BONEWEIGHT);
	glVertexAttribPointer(ATTRIB_BONEWEIGHT, 4, GL_FLOAT, 0, 0, 0);
	this->boneweight = vbo;
}

void GLVAO::setTangent(GLuint vbo)
{
	glEnableVertexAttribArray(ATTRIB_TANGENT);
	glVertexAttribPointer(ATTRIB_TANGENT, 3, GL_FLOAT, 0, 0, 0);
	this->tangent = vbo;
}


/**
* Bind VAO for rendering
* If we are using software fallback, binds all the VBOs instead
**/
void GLVAO::bind()
{
	glBindVertexArray(this->vao);
}
