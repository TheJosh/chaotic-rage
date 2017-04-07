// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once


/**
* Include OpenGL or OpenGL ES on various platforms
**/
#define OpenGL
#define USE_DEBUG_DRAW
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>


// Constants for VBOs <-> Shaders
// (glBindAttribLocation)
#define ATTRIB_POSITION 0         // vPosition
#define ATTRIB_NORMAL 1           // vNormal
#define ATTRIB_TEXUV 2            // vTexUV0, vTexUV1
#define ATTRIB_BONEID 4           // vBoneIDs
#define ATTRIB_BONEWEIGHT 5       // vBoneWeights
#define ATTRIB_TEXTCOORD 6        // vCoord
#define ATTRIB_COLOR 7            // vColor
#define ATTRIB_TANGENT 8          // vTangent
#define ATTRIB_BITANGENT 9        // vBitangent

// Shader IDs
// TODO: Should this be an enum instead?
#define SHADER_BASIC 0
#define SHADER_SKYBOX 1
#define SHADER_ENTITY_STATIC 2
#define SHADER_ENTITY_BONES 3
#define SHADER_TERRAIN_PLAIN 4
#define SHADER_TERRAIN_NORMALMAP 5
#define SHADER_TERRAIN_SPLAT 9
#define SHADER_WATER 6
#define SHADER_TEXT 7
#define SHADER_ENTITY_STATIC_BUMP 8
#define SHADER_LIGHT_PASS 10
