// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <glm/glm.hpp>
#include <string>
#include "../rage.h"
#include "../render/sprite.h"

using namespace std;


#define TEXTURE_SPLAT_LAYERS 4


class Mod;
class btRigidBody;
class RendererHeightmap;
class GLShader;


class TextureSplatLayer
{
	public:
		/**
		* Main texture
		**/
		SpritePtr texture;

		/**
		* Scale for main texture
		**/
		float scale;

		/**
		* Should this texture be doubled up?
		**/
		bool dbl;

		/**
		* Detail texture
		**/
		SpritePtr detail;
		float detail_scale;

	public:
		TextureSplatLayer() :
			texture(NULL), scale(128.0f), dbl(false), detail(NULL), detail_scale(4.0f)
			{};
};


/**
* A 3D struture represented by a 2D array of height (Y) values.
**/
class Heightmap {
	friend class Map;
	friend class RendererHeightmap;

	public:
		/**
		 * Max size for data, per axis
		 * Total max size will be 8192 x 8192 = ~67 million cells
		 */
		static const int MAX_DATA_SIZE = 8192;

	protected:
		// Physical size
		float sizeX;
		float sizeZ;

		// Data array size
		int sx;
		int sz;

		// Scale on Y axis
		float scale;

		glm::vec3 pos;

		// Heights array
		float* data;

		// Physics object
		btRigidBody* ground;

		// BigTexture (note - these leak)
		SpritePtr terrain;
		SpritePtr normal;

		// Texture splatting
		SpritePtr alphamap;
		TextureSplatLayer layers[TEXTURE_SPLAT_LAYERS];

		// Lightmaps (only supported by texture splat at the moment)
		SpritePtr lightmap;

	public:
		RendererHeightmap* renderer;

	public:
		Heightmap(float sizeX, float sizeZ, float scale, glm::vec3 pos)
			: sizeX(sizeX), sizeZ(sizeZ), sx(0), sz(0), scale(scale), pos(pos), data(NULL), ground(NULL), terrain(NULL),
			normal(NULL), alphamap(NULL), layers(), lightmap(NULL)
			{};

		~Heightmap();

	public:
		/**
		* Load height data from an image
		**/
		float* loadIMG(Mod* mod, string filename, int *imW, int *imH);

		/**
		* Load height data from an image
		**/
		float* loadRAW16(Mod* mod, string filename, int sx, int sz);

		/**
		* Load height data from a HFZ file
		**/
		bool loadHFZ(Mod* mod, string filename);

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
		* Return the bigtexture if set
		**/
		SpritePtr getBigNormal() { return normal; }

		/**
		* Return the lightmap if set
		**/
		SpritePtr getLightmap() { return lightmap; }

		/**
		* Return the coordinate to position the center of the heightmap
		**/
		glm::vec3 getPosition() { return pos; }

		/**
		* Are we all loaded up?
		**/
		bool isValid() { return (data != NULL); }

		/**
		* Get the value from the heightmap array
		**/
		float getValue(int X, int Z);

		/**
		* Get the physical size on the X/Z axis
		**/
		float getSizeX() { return this->sizeX; }
		float getSizeZ() { return this->sizeZ; }

		/**
		* Get the data array size on the X/Z axis
		**/
		unsigned int getDataSizeX() { return this->sx; }
		unsigned int getDataSizeZ() { return this->sz; }

		/**
		* Get the scale on the X/Y/Z axis
		**/
		float getScaleX();
		float getScaleY();
		float getScaleZ();
};

