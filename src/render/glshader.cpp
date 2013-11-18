// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <algorithm>
#include <map>
#include "gl.h"
#include "../rage.h"
#include "glshader.h"

using namespace std;



/**
* Return the shader program id
**/
GLuint GLShader::p()
{
	return this->program;
}


/**
* Return a uniform location id
**/
GLuint GLShader::uniform(const char* name)
{
	map<const char*, GLuint>::iterator it = this->uniforms.find(name);
	if (it != this->uniforms.end()) {
		return it->second;
	}
	
	GLuint loc = glGetUniformLocation(this->program, name);
	
	this->uniforms.insert(std::pair<const char*, GLuint>(name, loc));
	
	return loc;
}

