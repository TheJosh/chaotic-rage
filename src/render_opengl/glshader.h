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
		* Creates and compile an OpenGL "shader" object from a single string of GLSL code
		* Prepends the shader source code with a GLSL version string
		*
		* @param GLenum type Shader type, e.g. GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
		* @param char* code NULL-terminated string of GLSL code
		* @return GLuint
		**/
		GLuint createShader(GLenum type, const char* code);

		/**
		* Creates and compile an OpenGL "shader" object from a one or more strings of GLSL code
		*
		* @param GLenum type Shader type, e.g. GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
		* @param GLsizei count Number of elements in the 'code' array
		* @param char** code Array of NULL-terminated strings of GLSL code
		* @return GLuint
		**/
		GLuint createShader(GLenum type, const GLsizei count, const char** code);

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

		/**
		* Load GLSL code from a shader source file in the base mod
		* Don't forget to free when done.
		**/
		static char* loadCodeFile(std::string filename);


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

