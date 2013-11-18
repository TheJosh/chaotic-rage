// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once


/**
* Include in OpenGL or OpenGL ES on various platforms
**/
#if defined(__ANDROID__)
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
	
#elif defined(__WIN32__)
	#include <GL/glew.h>
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glext.h>
	
#else
	#include <GL/glew.h>
	#include <GL/gl.h>
	#include <GL/glu.h>
#endif

