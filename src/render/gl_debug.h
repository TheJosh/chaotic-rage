// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once



/**
* For reporting errors
**/
#if defined(RELEASE)
	#define CHECK_OPENGL_ERROR
	
#elif defined(GLES)
	#define CHECK_OPENGL_ERROR \
	{	GLenum error; \
		error = glGetError(); \
		if (error != GL_NO_ERROR) { \
			char buf[255]; \
			switch (error) { \
				case GL_INVALID_ENUM: sprintf(buf, "OpenGL ES error: GL_INVALID_ENUM at line %i of %s", __LINE__, __FILE__); \
				case GL_INVALID_VALUE: sprintf(buf, "OpenGL ES error: GL_INVALID_VALUE at line %i of %s", __LINE__, __FILE__); \
				case GL_INVALID_OPERATION: sprintf(buf, "OpenGL ES error: GL_INVALID_OPERATION at line %i of %s", __LINE__, __FILE__); \
				case GL_INVALID_FRAMEBUFFER_OPERATION: sprintf(buf, "OpenGL ES error: GL_INVALID_FRAMEBUFFER_OPERATION at line %i of %s", __LINE__, __FILE__); \
				case GL_OUT_OF_MEMORY: sprintf(buf, "OpenGL ES error: GL_OUT_OF_MEMORY at line %i of %s", __LINE__, __FILE__); \
				default: sprintf(buf, "Unknown OpenGL ES error at line %i of %s", __LINE__, __FILE__); \
			} \
			reportFatalError(std::string(buf)); \
		} \
	}
	
#elif defined(OpenGL)
	#define CHECK_OPENGL_ERROR \
	{	GLenum error; \
		error = glGetError(); \
		if (error != GL_NO_ERROR) { \
			cerr << "OpenGL Error:\n"; \
			while (error) { \
				cerr << " - " << gluErrorString(error) << "\n"; \
				error = glGetError(); \
			} \
			cerr << "Location: " << __FILE__ << ":" << __LINE__ << "\n"; \
			reportFatalError("OpenGL error"); \
		} \
	}
	
#endif
