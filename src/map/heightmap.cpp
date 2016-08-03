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
#include "../libhfz/libhfz.h"


using namespace std;


/**
* Take the heightmap image and turn it into a data array
**/
float* Heightmap::loadIMG(Mod* mod, string filename, int *imW, int *imH)
{
	int nX, nZ;
	Uint8 r,g,b;
	SDL_RWops *rw;
	SDL_Surface *surf;

	// Create rwops
	rw = mod->loadRWops(filename);
	if (rw == NULL) {
		return NULL;
	}

	// Load image file
	surf = IMG_Load_RW(rw, 0);
	if (surf == NULL) {
		SDL_RWclose(rw);
		return NULL;
	}

	if (imW) *imW = surf->w;
	if (imH) *imH = surf->h;

	if (surf->w > Heightmap::MAX_DATA_SIZE || surf->h > Heightmap::MAX_DATA_SIZE) {
		SDL_RWclose(rw);
		return NULL;
	}

	float* data = new float[surf->w * surf->h];
	if (!data) {
		return NULL;
	}

	for (nZ = 0; nZ < this->sz; nZ++) {
		for (nX = 0; nX < this->sx; nX++) {

			Uint32 pixel = getPixel(surf, nX, nZ);
			SDL_GetRGB(pixel, surf->format, &r, &g, &b);

			data[nZ * this->sx + nX] = r / 255.0f * this->scale;

		}
	}

	SDL_RWclose(rw);
	SDL_FreeSurface(surf);
	return data;
}


/**
* Take a raw heightmap image and load it into the data array.
* Input should be 16-bit ints.
* If exporting from L3DT, use 16-bit full scale.
**/
float* Heightmap::loadRAW16(Mod* mod, string filename, int sx, int sz)
{
	Sint64 len;

	if (sx > Heightmap::MAX_DATA_SIZE || sz > Heightmap::MAX_DATA_SIZE) {
		return NULL;
	}
	
	Uint8* binary = mod->loadBinary(filename, &len);
	if (len != (2 * sx * sz)) {
		free(binary);
		return NULL;
	}

	float* data = new float[sx * sz];
	if (!data) {
		free(binary);
		return NULL;
	}

	Uint8 *in = binary;
	float* out = data;
	while (len) {
		unsigned int val = (in[1]<<8) | in[0];
		*out++ = (float)val / 65535.0f * this->scale;
		in += 2; len -= 2;
	}

	free(binary);
	return data;
}


/**
* Take a raw heightmap image and load it into the data array.
* Input should be 16-bit ints.
* If exporting from L3DT, use 16-bit full scale.
**/
bool Heightmap::loadHFZ(Mod* mod, string filename)
{
	float* hfdata;
	hfzHeader fh;

	filename = mod->getRealFilename(filename);

	// load the file
	long rval = hfzLoadEx(filename.c_str(), fh, &hfdata, NULL, NULL);
	if (LIBHFZ_STATUS_OK != rval) {
		reportFatalError(
			"Error when loading hfz: " + std::string(hfzGetErrorStr(rval))
		);
	}

	this->sx = fh.nx;
	this->sz = fh.ny;
	hfzHeader_Reset(fh);

	this->data = new float[this->sx * this->sz];
	if (!data) {
		return false;
	}

	// HFZ files use a different coordinate system, so flip the Y-axis
	int src_z = this->sz - 1;
	int dst_z = 0;
	for (; src_z >= 0; --src_z, ++dst_z) {
		for (int x = 0; x < this->sx; ++x) {
			data[x + dst_z * this->sx] = hfdata[x + src_z * this->sx];
		}
	}

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
		this->getScaleX(),
		1.0f,
		this->getScaleZ()
	));

	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(
		btQuaternion(0, 0, 0, 1),
		btVector3(pos.x, this->scale/2.0f - pos.y, pos.z)
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
	return (float)sizeX / ((float)sx - 1.0f);
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
	return (float)sizeZ / ((float)sz - 1.0f);
}


/**
* Cleanup
**/
Heightmap::~Heightmap()
{
	delete [] data;
	delete ground;
	
	// TODO: How to free sprites without a ptr to the renderer?
}

