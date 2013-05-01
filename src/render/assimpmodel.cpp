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
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
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
	assert(this->rootNode != NULL);
}


/**
* Load a node (and it's children) from the node tree
**/
AssimpNode* AssimpModel::loadNode(aiNode* nd)
{
	if (nd->mNumMeshes == 0 && nd->mNumChildren == 0) return NULL;

	unsigned int i;
	AssimpNode* myNode = new AssimpNode();
	myNode->name = std::string(nd->mName.C_Str());

	for (i = 0; i < nd->mNumMeshes; i++) {
		myNode->meshes.push_back(nd->mMeshes[i]);
	}
	
	for (i = 0; i < nd->mNumChildren; i++) {
		AssimpNode* child = loadNode(nd->mChildren[i]);
		if (child != NULL) myNode->addChild(child);
	}
	
	aiMatrix4x4 m = nd->mTransformation;
	m.Transpose();
	myNode->transform = glm::make_mat4((float *) &m);
	
	return myNode;
}


/**
* Find a node by name
**/
AssimpNode* AssimpModel::findNode(AssimpNode* nd, string name)
{
	if (nd->name == name) return nd;

	for (vector<AssimpNode*>::iterator it = nd->children.begin(); it != nd->children.end(); it++) {
		AssimpNode* maybe = this->findNode((*it), name);
		if (maybe) return maybe;
	}

	return NULL;
}


/**
* Load the animations for this assimp model
**/
void AssimpModel::loadAnimations()
{
	unsigned int n;
	
	for (n = 0; n < sc->mNumAnimations; n++) {
		const aiAnimation* pAnimation = sc->mAnimations[n];
		
		AssimpAnimation* loaded = this->loadAnimation(pAnimation);
		if (loaded) {
			this->animations.push_back(loaded);
		}
	}
}


/**
* Load a single animation
**/
AssimpAnimation* AssimpModel::loadAnimation(const aiAnimation* anim)
{
	unsigned int n, m;
	AssimpAnimKey key;
	AssimpAnimation *out;

	out = new AssimpAnimation();
	out->name = std::string(anim->mName.C_Str());
	out->ticsPerSec = anim->mTicksPerSecond;
	out->duration = anim->mDuration;

	// Load animation channels
	out->anims.reserve(anim->mNumChannels);
	for (n = 0; n < anim->mNumChannels; n++) {
		const aiNodeAnim* pNodeAnim = anim->mChannels[n];

		AssimpNodeAnim* na = new AssimpNodeAnim();
		na->name = std::string(pNodeAnim->mNodeName.C_Str());

		// Positions
		na->position.reserve(pNodeAnim->mNumPositionKeys);
		for (m = 0; m < pNodeAnim->mNumPositionKeys; m++) {
			key.time = pNodeAnim->mPositionKeys[m].mTime;
			key.vec = this->convVector(pNodeAnim->mPositionKeys[m].mValue);
			na->position.push_back(key);
		}

		// Rotations
		na->rotation.reserve(pNodeAnim->mNumRotationKeys);
		for (m = 0; m < pNodeAnim->mNumRotationKeys; m++) {
			key.time = pNodeAnim->mRotationKeys[m].mTime;
			key.quat = this->convQuaternion(pNodeAnim->mRotationKeys[m].mValue);
			na->rotation.push_back(key);
		}

		// Scales
		na->scale.reserve(pNodeAnim->mNumScalingKeys);
		for (m = 0; m < pNodeAnim->mNumScalingKeys; m++) {
			key.time = pNodeAnim->mScalingKeys[m].mTime;
			key.vec = this->convVector(pNodeAnim->mScalingKeys[m].mValue);
			na->scale.push_back(key);
		}

		out->anims.push_back(na);
	}

	return out;
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


/**
* Convert a vector to our internal format
**/
glm::vec3 AssimpModel::convVector(aiVector3D in)
{
	return glm::vec3(in.x, in.y, in.z);
}


/**
* Convert a vector to our internal format
**/
glm::quat AssimpModel::convQuaternion(aiQuaternion in)
{
	return glm::quat(in.w, in.x, in.y, in.z);
}


/**
* Calculate the bone ids and bone weights
**/
void AssimpModel::loadBones(const aiMesh* mesh, AssimpMesh* myMesh)
{
	unsigned int n, m;
	unsigned int *idx;

	// Allocate space for the IDs and weights
	this->boneIds = (unsigned int*) malloc(sizeof(unsigned int) * 4 * mesh->mNumVertices);
	this->boneWeights = (float*) malloc(sizeof(float) * 4 * mesh->mNumVertices);
	memset(this->boneIds, 0, sizeof(unsigned int) * mesh->mNumVertices);
	memset(this->boneWeights, 0, sizeof(float) * 4 * mesh->mNumVertices);

	// Keeps track of which index a given vertex is up to
	idx = (unsigned int*) malloc(sizeof(unsigned int) * mesh->mNumVertices);
	memset(idx, 0, sizeof(unsigned int) * mesh->mNumVertices);

	cout << "Doing bones for mesh " << myMesh << "\n";
	
	// Loop through the weights of all the bones
	// Save the id and the weight in the arrays as required
	for (n = 0; n < mesh->mNumBones; n++) {
		aiBone* bone = mesh->mBones[n];
		
		for (m = 0; m < bone->mNumWeights; m++) {
			aiVertexWeight w = bone->mWeights[m];

			if (idx[w.mVertexId] == MAX_WEIGHTS) continue;

			this->boneIds[w.mVertexId * 4 + idx[w.mVertexId]] = n;
			this->boneWeights[w.mVertexId * 4 + idx[w.mVertexId]] = w.mWeight;

			idx[w.mVertexId]++;
		}

		// We also have to save the bone data in our structure
		AssimpBone* b = new AssimpBone();
		b->name = std::string(bone->mName.C_Str());
		b->offset = glm::make_mat4((float *) &(bone->mOffsetMatrix));
		myMesh->bones.push_back(b);
		
		cout << "    Found bone " << n << "; name = " << b->name << "\n";
	}

	free(idx);
}


/**
* Returns an array where each index refers to four
* ints (which get mapped to a glm::ivec4) of bone ids
* The index is the vertex index
**/
unsigned int* AssimpModel::getBoneIds()
{
	return this->boneIds;
}


/**
* Returns an array where each index refers to four
* floats (which get mapped to a glm::vec4) of bone weights
* The index is the vertex index
**/
float* AssimpModel::getBoneWeights()
{
	return this->boneWeights;
}


/**
* Free the bone data loaded in loadBones(
**/
void AssimpModel::freeBones()
{
	free(this->boneIds);
	free(this->boneWeights);
}
