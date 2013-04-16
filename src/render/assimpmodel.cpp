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
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "assimpmodel.h"
#include "../rage.h"

using namespace std;


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
	cout << "Assimp model: " << this->name << "\n";
	Assimp::Importer importer;

	unsigned int flags = aiProcess_CalcTangentSpace
		| aiProcess_Triangulate
		| aiProcess_JoinIdenticalVertices
		| aiProcess_SortByPType
		| aiProcess_FlipUVs;
	
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
	this->loadNodes();
	this->loadAnimations();
	
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
	aiMatrix4x4 trafo;
	aiIdentityMatrix4(&trafo);
	
	this->boundingSize = this->calcBoundingSizeNode(sc->mRootNode, &trafo);
}


/**
* Returns the bounding size of the mesh of the first frame
**/
btVector3 AssimpModel::calcBoundingSizeNode(const struct aiNode* nd, aiMatrix4x4* trafo)
{
	float x = 0.0f, y = 0.0f, z = 0.0f;
	unsigned int n = 0, t;
	aiMatrix4x4 prev = *trafo;
	
	aiMultiplyMatrix4(trafo, &nd->mTransformation);
	
	for (; n < nd->mNumMeshes; ++n) {
		const struct aiMesh* mesh = sc->mMeshes[nd->mMeshes[n]];
		
		for (t = 0; t < mesh->mNumVertices; ++t) {
			aiVector3D tmp = mesh->mVertices[t];
			aiTransformVecByMatrix4(&tmp, trafo);
			
			x = MAX(x, tmp.x);
			y = MAX(y, tmp.y);
			z = MAX(z, tmp.z);
		}
	}
	
	for (n = 0; n < nd->mNumChildren; ++n) {
		btVector3 size = this->calcBoundingSizeNode(nd->mChildren[n], trafo);
		x = MAX(x, size.x());
		y = MAX(y, size.y());
		z = MAX(z, size.z());
	}
	
	*trafo = prev;
	return btVector3(x, y, z);
}


/**
* Load the node tree
**/
void AssimpModel::loadNodes()
{
	this->rootNode = this->loadNode(this->sc->mRootNode);
}


/**
* Load a node (and it's children) from the node tree
**/
AssimpNode* AssimpModel::loadNode(aiNode* nd)
{
	unsigned int i;
	AssimpNode* myNode = new AssimpNode();
	
	for (i = 0; i < nd->mNumMeshes; i++) {
		myNode->meshes.push_back(nd->mMeshes[i]);
	}
	
	for (i = 0; i < nd->mNumChildren; i++) {
		AssimpNode* child = loadNode(nd->mChildren[i]);
		myNode->addChild(child);
	}
	
	aiMatrix4x4 m = nd->mTransformation;
	m.Transpose();
	myNode->transform = glm::make_mat4((float *) &m);
	
	return myNode;
}


/**
* Load the animations for this assimp model
**/
void AssimpModel::loadAnimations()
{
	unsigned int n;
	
	for (n = 0; n < sc->mNumAnimations; n++) {
		const aiAnimation* pAnimation = sc->mAnimations[n];
		
		cout << "Assimp animation: " << std::string(pAnimation->mName.C_Str()) << "\n";
		cout << "Assimp tickspersecond: " << pAnimation->mTicksPerSecond << "\n";
		
		// TODO: Actually load stuff
		
	}
}


/**
* Returns the bounding size of the mesh of the first frame
* HE = half extents
**/
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


