// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <algorithm>
#include <map>
#include "gl.h"
#include "gl_debug.h"
#include "../rage.h"
#include "glshader.h"
#include "../game_engine.h"
#include "../mod/mod.h"
#include "../mod/mod_manager.h"

using namespace std;


/**
* Creates and compile an OpenGL "shader" object from a single string of GLSL code
* Prepends the shader source code with a GLSL version string
*
* @param GLenum type Shader type, e.g. GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
* @param char* code NULL-terminated string of GLSL code
* @return GLuint
**/
GLuint GLShader::createShader(GLenum type, const char* code)
{
	const char* strings[2];

	strings[0] = "#version 130\n";
	strings[1] = code;

	return this->createShader(type, 2, strings);
}


/**
* Creates and compile an OpenGL "shader" object from a one or more strings of GLSL code
*
* @param GLenum type Shader type, e.g. GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
* @param GLsizei count Number of elements in the 'code' array
* @param char** code Array of NULL-terminated strings of GLSL code
* @return GLuint
**/
GLuint GLShader::createShader(GLenum type, const GLsizei count, const char** code)
{
	GLint success;

	GLuint shader = glCreateShader(type);
	if (shader == 0) {
		return 0;
	}

	glShaderSource(shader, count, code, NULL);

	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (success == 0) {
		glDeleteShader(shader);
		GLchar InfoLog[1024];
		glGetShaderInfoLog(shader, 1024, NULL, InfoLog);
		string log = string(InfoLog);
		log = replaceString(log, "\n", "\n\t");
		log = trimString(log);
		GL_LOG("Error compiling shader:\n\t%s", log.c_str());
		return 0;
	}

	return shader;
}


/**
* Create and link a shader program from two compiled shaders
* Does NOT delete the linked shaders - calling code needs to do this
* @return True on success, false on failure
**/
bool GLShader::createProgFromShaders(GLuint vertex, GLuint fragment)
{
	GLint success;

	// Create program object
	GLuint program = glCreateProgram();
	if (program == 0) {
		return false;
	}

	glAttachShader(program, vertex);
	glAttachShader(program, fragment);

	// Bind attribs
	glBindAttribLocation(program, ATTRIB_POSITION, "vPosition");
	glBindAttribLocation(program, ATTRIB_NORMAL, "vNormal");
	glBindAttribLocation(program, ATTRIB_TEXUV + 0, "vTexUV0");
	glBindAttribLocation(program, ATTRIB_TEXUV + 1, "vTexUV1");
	glBindAttribLocation(program, ATTRIB_BONEID, "vBoneIDs");
	glBindAttribLocation(program, ATTRIB_BONEWEIGHT, "vBoneWeights");
	glBindAttribLocation(program, ATTRIB_TEXTCOORD, "vCoord");
	glBindAttribLocation(program, ATTRIB_COLOR, "vColor");
	glBindAttribLocation(program, ATTRIB_TANGENT, "vTangent");
	glBindAttribLocation(program, ATTRIB_BITANGENT, "vBitangent");

	// Link
	glLinkProgram(program);

	// Check link worked
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (! success) {
		GLchar infolog[1024];
		glGetProgramInfoLog(program, 1024, NULL, infolog);
		GL_LOG("Error linking program\n%s", infolog);
		return false;
	}

	CHECK_OPENGL_ERROR;

	this->program = program;
	return true;
}


/**
* Creates and link a shader program from two shader code strings
* @return True on success, false on failure
**/
bool GLShader::createProgFromStrings(const char* vertex, const char* fragment)
{
	GLuint sVertex, sFragment;
	bool result;

	sVertex = this->createShader(GL_VERTEX_SHADER, vertex);
	if (sVertex == 0) {
		GL_LOG("Invalid vertex shader (program %u)", program);
		return false;
	}

	sFragment = this->createShader(GL_FRAGMENT_SHADER, fragment);
	if (sFragment == 0) {
		glDeleteShader(sVertex);
		GL_LOG("Invalid fragment shader (program %u)", program);
		return false;
	}

	result = this->createProgFromShaders(sVertex, sFragment);

	glDeleteShader(sVertex);
	glDeleteShader(sFragment);

	return result;
}


/**
* Load GLSL code from a shader source file in the base mod
**/
char* GLShader::loadCodeFile(std::string filename)
{
	Mod* mod = GEng()->mm->getBase();
	return mod->loadText("shaders_gl/" + filename);
}


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
