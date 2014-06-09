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


/**
* A 3D struture represented by a 2D array of height (Y) values.
**/
class Heightmap {
	public:
		int width;
		int height;
		int sx;
		int sz;
		float scale;
		float* data;
		btRigidBody* ground;

	public:
		Heightmap(int width, int height)
			: width(width), height(height), sx(0), sz(0), scale(0.0f), data(NULL), ground(NULL)
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
		bool createRigidBody(float mapSX, float mapSZ);

		/**
		* Get the value from the heightmap array
		**/
		float getValue(int X, int Z);

		/**
		* Get the scale on the X/Y/Z axis
		**/
		float getScaleX();
		float getScaleY();
		float getScaleZ();
};

