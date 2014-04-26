// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once

#include "../platform/platform.h"


/**
* Check OpenGL errors
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
				default: sprintf(buf, "Unknown OpenGL ES (%x) error at line %i of %s", error, __LINE__, __FILE__); \
			} \
			reportFatalError(std::string(buf)); \
		} \
	}
	
#elif defined(OpenGL)
	#define CHECK_OPENGL_ERROR \
	{	GLenum error; \
		error = glGetError(); \
		if (error != GL_NO_ERROR) { \
			char buf[255]; \
			sprintf(buf, "OpenGL error: %s at line %i of %s", (const char*) gluErrorString(error), __LINE__, __FILE__); \
			reportFatalError(std::string(buf)); \
		} \
	}
	
#endif


/**
* For GL logging and errors
**/
#if defined(__ANDROID__)
	#include <android/log.h>
	#define GL_LOG(msg, ...) __android_log_print(ANDROID_LOG_ERROR, "ChaoticRage", "GL Log: " msg, __VA_ARGS__)

#elif defined(__WIN32__)
	#define GL_LOG(msg, ...) \
	{ \
		char buf[2048]; \
		sprintf(buf, "GL Log: " msg "\n", __VA_ARGS__); \
		displayMessageBox(std::string(buf)); \
	}

#else
	#define GL_LOG(msg, ...) fprintf(stderr, "GL Log: " msg "\n", __VA_ARGS__)

#endif

