// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <map>
#include "gl.h"
#include "../rage.h"



/**
* A simple class for managing the IDs of shader programs, uniforms, etc.
**/
class GLShader
{
	private:
		// The shader program
		GLuint program;
		
		// Cached list of uniform locations
		map<const char*, GLuint> uniforms;
		
	public:
		GLShader(const GLuint p) : program(p) {}
		
		GLuint p();
		GLuint uniform(const char* name);
};

