// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <string>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include <map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "assimpmodel.h"
#include "../rage.h"

using namespace std;


/**
* Our importer
**/
static Assimp::Importer importer;


/**
* Create a model. It's not loaded yet, use load() to load it.
**/
AssimpModel::AssimpModel(Mod* mod, string name)
{
	this->sc = NULL;
	this->mod = mod;
	this->name = name;
	this->vao = 0;
}


/**
* Load the model.
* Return true on success, false on failure.
**/
bool AssimpModel::load()
{
	unsigned int flags = aiProcess_CalcTangentSpace
		| aiProcess_Triangulate
		| aiProcess_JoinIdenticalVertices
		| aiProcess_SortByPType;
	
	int len;
	Uint8 * data = this->mod->loadBinary("models/" + this->name, &len);
	if (! data) {
		cout << "Failed to load " << this->name << "; file read failed\n";
		return false;
	}
	
	this->sc = importer.ReadFileFromMemory((const void*) data, len, flags, this->name.c_str());
	if (! this->sc) {
		cout << "Failed to load " << this->name << "; " << importer.GetErrorString() << "\n";
		return false;
	}
	
	free(data);
	
	return true;
}


/**
* Returns the bounding size of the mesh of the first frame
**/
btVector3 AssimpModel::getBoundingSize()
{
	float x = 0.0f, y = 0.0f, z = 0.0f;
	unsigned int n = 0, t;
	
	if (sc == NULL) return btVector3(0.0f, 0.0f, 0.0f);
	
	for (; n < sc->mRootNode->mNumMeshes; ++n) {
		const struct aiMesh* mesh = sc->mMeshes[sc->mRootNode->mMeshes[n]];
		
		for (t = 0; t < mesh->mNumVertices; ++t) {
			aiVector3D tmp = mesh->mVertices[t];

			x = MAX(x, tmp.x);
			y = MAX(y, tmp.y);
			z = MAX(z, tmp.z);
		}
	}

	return btVector3(x, y, z);
}


/**
* Returns the bounding size of the mesh of the first frame
* HE = half extents
**/
btVector3 AssimpModel::getBoundingSizeHE()
{
	btVector3 ret = getBoundingSize();
	ret.setX(ret.x() / 2.0f);
	ret.setY(ret.y() / 2.0f);
	ret.setZ(ret.z() / 2.0f);
	return ret;
}


