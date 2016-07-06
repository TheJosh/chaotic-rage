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
		std::map<const char*, GLuint> uniforms;

		// Should the various lighting uniforms be set?
		bool uniforms_lighting;

	public:
		GLShader(const GLuint p) : program(p), uniforms_lighting(true) {}

		/**
		* Creates and compile an OpenGL "shader" object
		* @return GLuint
		**/
		GLuint GLShader::createShader(const char* code, GLenum type);

		/**
		* Create a shader from already-compiled shader objects
		* Does NOT delete the linked shaders - calling code needs to do this
		* @return True on success, false on failure
		**/
		bool createProgFromShaders(GLuint vertex, GLuint fragment);

		/**
		* Create a shader from strings containing GLSL code
		* @return True on success, false on failure
		**/
		bool createProgFromStrings(const char* vertex, const char* fragment);

		GLuint p();
		GLuint uniform(const char* name);
		
		
		/**
		* Enables or disables the flag for if lighting uniforms should be set
		**/
		void setUniformsLighting(bool flag)
		{
			this->uniforms_lighting = flag;
		}
		
		/**
		* Get the flag for if lighting uniforms should be set
		**/
		bool getUniformsLighting()
		{
			return this->uniforms_lighting;
		}
};

