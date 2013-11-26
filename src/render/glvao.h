// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <map>
#include "gl.h"
#include "../rage.h"



/**
* VAO wrapper with software fallback
**/
class GLVAO
{
	private:
		// VAO
		GLuint vao;
		
		// VBOs - separate
		GLuint index;
		GLuint position;
		GLuint normal;
		GLuint texuv;
		GLuint boneid;
		GLuint boneweight;
		GLuint tangent;
		
		// VBOs - interleaved
		GLuint interleaved_pnt;
		GLuint interleaved_pc;
		
	public:
		GLVAO();
		~GLVAO();

	public:
		// Set buffer ids - separate
		void setIndex(GLuint vbo);
		void setPosition(GLuint vbo);
		void setNormal(GLuint vbo);
		void setTexUV(GLuint vbo);
		void setBoneId(GLuint vbo);
		void setBoneWeight(GLuint vbo);
		void setTangent(GLuint vbo);
		
		// Set buffer ids - interleaved
		void setInterleavedPNT(GLuint vbo);
		void setInterleavedPC(GLuint vbo);
		
		// Get buffer id, for updating the data
		GLuint getInterleavedPNT() { return this->interleaved_pnt; }
		GLuint getInterleavedPC() { return this->interleaved_pc; }
		
		// Bind the VAO
		void bind();
		void unbind();
		
	private:
		void bindBuffers();
		void unbindBuffers();
};

