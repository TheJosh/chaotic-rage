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
	this->mod = mod;
	this->name = name;
	this->sc = NULL;
}


/**
* Load the model.
* Return true on success, false on failure.
**/
bool AssimpModel::load(Render3D* render)
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
	
	this->calcBoundingSize();
	
	if (! render->loadAssimpModel(this)) {
		return false;
	}
	
	this->sc = NULL;
	
	return true;
}


/**
* Returns the bounding size of the mesh of the first frame
**/
void AssimpModel::calcBoundingSize()
{
	this->boundingSize = this->calcBoundingSizeNode(sc->mRootNode);
}


/**
* Returns the bounding size of the mesh of the first frame
**/
btVector3 AssimpModel::calcBoundingSizeNode(const struct aiNode* nd)
{
	float x = 0.0f, y = 0.0f, z = 0.0f;
	unsigned int n = 0, t;
	
	if (sc == NULL) return btVector3(0.0f, 0.0f, 0.0f);
	
	for (; n < nd->mNumMeshes; ++n) {
		const struct aiMesh* mesh = sc->mMeshes[nd->mMeshes[n]];
		
		for (t = 0; t < mesh->mNumVertices; ++t) {
			aiVector3D tmp = mesh->mVertices[t];

			x = MAX(x, tmp.x);
			y = MAX(y, tmp.y);
			z = MAX(z, tmp.z);
		}
	}
	
	for (n = 0; n < nd->mNumChildren; ++n) {
		btVector3 size = this->calcBoundingSizeNode(nd->mChildren[n]);
		x = MAX(x, size.x());
		y = MAX(y, size.y());
		z = MAX(z, size.z());
	}
	
	return btVector3(x, y, z);
}




btVector3 AssimpModel::getBoundingSize()
{
	return this->boundingSize;
}


/**
* Returns the bounding size of the mesh of the first frame
* HE = half extents
**/
btVector3 AssimpModel::getBoundingSizeHE()
{
	return btVector3(this->boundingSize.x() / 2.0f, this->boundingSize.y() / 2.0f, this->boundingSize.z() / 2.0f);
}


