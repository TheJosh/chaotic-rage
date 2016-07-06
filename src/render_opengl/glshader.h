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

	protected:
		/**
		* Creates and compile an OpenGL "shader" object
		* @return GLuint
		**/
		GLuint GLShader::createShader(const char* code, GLenum type);

	public:
		GLShader(const GLuint p) : program(p), uniforms_lighting(true) {}

		/**
		* Create a shader from strings containing GLSL code
		* @return True on success, false on failure
		**/
		bool createFromStrings(const char* vertex, const char* fragment);

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

