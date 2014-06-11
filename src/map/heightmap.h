// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <glm/glm.hpp>
#include <string>
#include "../rage.h"

using namespace std;


class Mod;
class btRigidBody;
class GLVAO;


/**
* A 3D struture represented by a 2D array of height (Y) values.
**/
class Heightmap {
	private:
		// Physical size
		float sizeX;
		float sizeZ;

		// Data array size
		int sx;
		int sz;

		// Scale on Y axis
		float scale;

		// Data array
		float* data;

		// Physics object
		btRigidBody* ground;

		// BigTexture
		SpritePtr terrain;

	public:
		// The OpenGL renderer entirely manages these
		GLVAO* glvao;
		unsigned int glsize;

	public:
		Heightmap(float sizeX, float sizeZ, float scale)
			: sizeX(sizeX), sizeZ(sizeZ), sx(0), sz(0), scale(scale), data(NULL), ground(NULL), terrain(NULL)
			{};

		~Heightmap();

	public:
		/**
		* Load height data from an image
		**/
		bool loadIMG(Mod* mod, string filename);

		/**
		* Create regid body for bullet physics
		**/
		btRigidBody* createRigidBody();

		/**
		* Set a big texture image for the heightmap
		**/
		void setBigTexture(SpritePtr tex) { terrain = tex; }

		/**
		* Return the bigtexture if set
		**/
		SpritePtr getBigTexture() { return terrain; }

		/**
		* Are we all loaded up?
		**/
		bool isValid() { return (data != NULL); }

		/**
		* Get the value from the heightmap array
		**/
		float getValue(int X, int Z);

		/**
		* Get the size on the X/Z axis
		**/
		float getSizeX() { return this->sizeX; }
		float getSizeZ() { return this->sizeZ; }

		float getDataSizeX() { return this->sx; }
		float getDataSizeZ() { return this->sz; }

		/**
		* Get the scale on the X/Y/Z axis
		**/
		float getScaleX();
		float getScaleY();
		float getScaleZ();
};

