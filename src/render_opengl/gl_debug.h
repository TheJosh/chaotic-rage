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

#else
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
#if defined(__WIN32__)
	#define GL_LOG(msg, ...) \
	{ \
		char buf[2048]; \
		sprintf(buf, "GL Log: " msg "\n", __VA_ARGS__); \
		displayMessageBox(std::string(buf)); \
	}

#else
	#define GL_LOG(msg, ...) fprintf(stderr, "GL Log: " msg "\n", ##__VA_ARGS__)

#endif

