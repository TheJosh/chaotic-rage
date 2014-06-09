// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include <SDL.h>
#include <SDL_image.h>

#include "heightmap.h"

#include "../mod/mod.h"
#include "../util/sdl_util.h"
#include "../util/btStrideHeightfieldTerrainShape.h"


using namespace std;


static bool isPowerOfTwo (unsigned int x)
{
	return ((x != 0) && ((x & (~x + 1)) == x));
}


/**
* Take the heightmap image and turn it into a data array
*
* TODO: Rejig this so we don't use SDL (e.g. prebuild byte array)
**/
bool Heightmap::loadIMG(Mod* mod, string filename)
{
	int nX, nZ;
	Uint8 r,g,b;
	SDL_RWops *rw;
	SDL_Surface *surf;

	// Create rwops
	rw = mod->loadRWops(filename);
	if (rw == NULL) {
		return false;
	}

	// Load image file
	surf = IMG_Load_RW(rw, 0);
	if (surf == NULL) {
		SDL_RWclose(rw);
		return false;
	}

	// Heightmaps need to be powerOfTwo + 1
	if (!isPowerOfTwo(surf->w - 1) || !isPowerOfTwo(surf->h - 1)) {
		SDL_RWclose(rw);
		SDL_FreeSurface(surf);
		return false;
	}

	this->data = new float[surf->w * surf->h];
	this->sx = surf->w;
	this->sz = surf->h;

	for (nZ = 0; nZ < this->sz; nZ++) {
		for (nX = 0; nX < this->sx; nX++) {

			Uint32 pixel = getPixel(surf, nX, nZ);
			SDL_GetRGB(pixel, surf->format, &r, &g, &b);

			this->data[nZ * this->sx + nX] = r / 255.0f * this->scale;

		}
	}

	SDL_RWclose(rw);
	SDL_FreeSurface(surf);
	return true;
}


/**
* Create the "ground" for the map
**/
btRigidBody* Heightmap::createRigidBody()
{
	if (this->data == NULL) return NULL;

	bool flipQuadEdges = false;

	btFasterHeightfieldTerrainShape * groundShape = new btFasterHeightfieldTerrainShape(
		this->sx, this->sz, this->data,
		0,
		0.0f, this->scale,
		1, PHY_FLOAT, flipQuadEdges
	);

	groundShape->setLocalScaling(btVector3(
		sizeX / ((float)this->sx - 1.0f),
		1.0f,
		sizeZ / ((float)this->sz - 1.0f)
	));

	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(
		btQuaternion(0, 0, 0, 1),
		btVector3(sizeX/2.0f, this->scale/2.0f, sizeZ/2.0f)
	));

	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(
		0,
		groundMotionState,
		groundShape,
		btVector3(0,0,0)
	);

	this->ground = new btRigidBody(groundRigidBodyCI);
	this->ground->setRestitution(0.f);
	this->ground->setFriction(10.f);

	// Debugging for the terrain without needing to recompile
	if (!debug_enabled("terrain")) {
		this->ground->setCollisionFlags(this->ground->getCollisionFlags()|btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
	}

	return this->ground;
}



/**
* Get the value from the data array at a given X/Z coord
**/
float Heightmap::getValue(int X, int Z)
{
	return data[Z * sx + X];
}


/**
* Get the scale on the X axis
**/
float Heightmap::getScaleX()
{
	return (float)sizeX / (float)(sx - 1.0f);
}


/**
* Get the scale on the Y axis
**/
float Heightmap::getScaleY()
{
	return scale;
}


/**
* Get the scale on the Z axis
**/
float Heightmap::getScaleZ()
{
	return (float)sizeZ / (float)(sz - 1.0f);
}


/**
* Cleanup
**/
Heightmap::~Heightmap()
{
	delete [] data;
	delete ground;
}

