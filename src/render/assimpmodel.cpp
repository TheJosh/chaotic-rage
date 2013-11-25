// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <string>
#include <map>
#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "gl.h"
#include "gl_debug.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <btBulletDynamicsCommon.h>
#include "../rage.h"
#include "../mod/mod.h"
#include "assimpmodel.h"
#include "render_opengl.h"
#include "sprite.h"

using namespace std;


/**
* Create a model. It's not loaded yet, use load() to load it.
**/
AssimpModel::AssimpModel(Mod* mod, string name)
{
	this->mod = mod;
	this->name = name;
	this->sc = NULL;
	this->shape = NULL;
}


/**
* Free loaded data
**/
AssimpModel::~AssimpModel()
{
	delete(this->shape);
}


/**
* Load the model.
* Return true on success, false on failure.
**/
bool AssimpModel::load(Render3D* render, bool meshdata)
{
	Assimp::Importer importer;

	unsigned int flags = aiProcess_CalcTangentSpace
		| aiProcess_Triangulate
		| aiProcess_JoinIdenticalVertices
		| aiProcess_SortByPType
		| aiProcess_FlipUVs;
	
	Sint64 len;
	Uint8 * data = this->mod->loadBinary("models/" + this->name, &len);
	if (! data) {
		this->mod->setLoadErr("Failed to load %s; file read failed", this->name.c_str());
		return false;
	}
	
	this->sc = importer.ReadFileFromMemory((const void*) data, len, flags, this->name.c_str());
	if (! this->sc) {
		this->mod->setLoadErr("Failed to load %s; file read failed; %s", this->name.c_str(), importer.GetErrorString());
		return false;
	}
	
	free(data);
	
	if (render != NULL and render->is3D()) {
		this->loadMeshes(true);
		this->loadMaterials(render);
	} else {
		this->loadMeshes(false);
	}
	
	if (meshdata) {
		this->loadMeshdata(render != NULL);
	}
	
	this->loadNodes();
	this->loadAnimations();
	this->calcBoundingBox();
	this->setBoneNodes();
	
	this->sc = NULL;
	
	return true;
}


/**
* Returns the bounding size of the mesh of the first frame
**/
void AssimpModel::calcBoundingBox()
{
	aiMatrix4x4 trafo;
	aiIdentityMatrix4(&trafo);
	aiVector3D min, max;

	// Calculate bounds and size
	min.x = min.y = min.z = 1e10f;
	max.x = max.y = max.z = -1e10f;
	this->calcBoundingBoxNode(sc->mRootNode, &min, &max, &trafo);
	boundingSize = btVector3(max.x - min.x, max.y - min.y, max.z - min.z);
}


/**
* Returns the bounding size of the mesh of the first frame
**/
void AssimpModel::calcBoundingBoxNode(const aiNode* nd, aiVector3D* min, aiVector3D* max, aiMatrix4x4* trafo)
{
	aiMatrix4x4 prev;
	unsigned int n = 0, t;

	prev = *trafo;
	aiMultiplyMatrix4(trafo, &nd->mTransformation);

	for (; n < nd->mNumMeshes; ++n) {
		const struct aiMesh* mesh = sc->mMeshes[nd->mMeshes[n]];

		// If the material name contains NOPHYSICS, we don't count the mesh towards the bounding size
		aiMaterial* material = sc->mMaterials[mesh->mMaterialIndex];
		if (material) {
			aiString name;
			material->Get(AI_MATKEY_NAME, name);
			if (strstr(name.C_Str(), "NOPHYSICS") != NULL) continue;
		}

		// Get the min and max vertexes for the mesh
		for (t = 0; t < mesh->mNumVertices; ++t) {
			aiVector3D tmp = mesh->mVertices[t];
			aiTransformVecByMatrix4(&tmp, trafo);

			min->x = MIN(min->x, tmp.x);
			min->y = MIN(min->y, tmp.y);
			min->z = MIN(min->z, tmp.z);

			max->x = MAX(max->x, tmp.x);
			max->y = MAX(max->y, tmp.y);
			max->z = MAX(max->z, tmp.z);
		}
	}

	// Calculate for children nodes too
	for (n = 0; n < nd->mNumChildren; ++n) {
		this->calcBoundingBoxNode(nd->mChildren[n], min, max, trafo);
	}

	*trafo = prev;
}


/**
* Load a model into a VAO, VBOs etc.
**/
void AssimpModel::loadMeshes(bool opengl)
{
	unsigned int n = 0;
	GLuint buffer;

	for (; n < sc->mNumMeshes; ++n) {
		const struct aiMesh* mesh = sc->mMeshes[n];
		AssimpMesh *myMesh = new AssimpMesh();
		
		myMesh->numFaces = mesh->mNumFaces;
		myMesh->materialIndex = mesh->mMaterialIndex;
		this->meshes.push_back(myMesh);
		
		if (! opengl) continue;
		
		// VAO
		myMesh->vao = new GLVAO();

		// Prep face array for VBO
		unsigned int *faceArray;
		faceArray = (unsigned int *)malloc(sizeof(unsigned int) * mesh->mNumFaces * 3);
		unsigned int faceIndex = 0;
		
		// Copy face data
		for (unsigned int t = 0; t < mesh->mNumFaces; ++t) {
			const struct aiFace* face = &mesh->mFaces[t];
			
			memcpy(&faceArray[faceIndex], face->mIndices, 3 * sizeof(int));
			faceIndex += 3;
		}
		
		// Faces
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh->mNumFaces * 3, faceArray, GL_STATIC_DRAW);
		free(faceArray);
		myMesh->vao->setIndex(buffer);

		// Positions
		if (mesh->HasPositions()) {
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*mesh->mNumVertices, mesh->mVertices, GL_STATIC_DRAW);
			myMesh->vao->setPosition(buffer);
		}
		
		// Normals
		if (mesh->HasNormals()) {
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*mesh->mNumVertices, mesh->mNormals, GL_STATIC_DRAW);
			myMesh->vao->setNormal(buffer);
		}
		
		// UVs
		if (mesh->HasTextureCoords(0)) {
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*mesh->mNumVertices, mesh->mTextureCoords[0], GL_STATIC_DRAW);
			myMesh->vao->setTexUV(buffer);
		}
		
		// Bone IDs and Weights
		if (mesh->HasBones()) {
			this->loadBones(mesh, myMesh);

			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int)*4*mesh->mNumVertices, this->boneIds, GL_STATIC_DRAW);
			myMesh->vao->setBoneId(buffer);

			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*mesh->mNumVertices, this->boneWeights, GL_STATIC_DRAW);
			myMesh->vao->setBoneWeight(buffer);

			this->freeBones();
		}

		// Tangent s and Bittangents, for normal mapping
		// TODO: Determine if there actually is a normal map in use on this mesh!
		if (mesh->HasTangentsAndBitangents()) {
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*mesh->mNumVertices, mesh->mTangents, GL_STATIC_DRAW);
			myMesh->vao->setTangent(buffer);
		}
		
		CHECK_OPENGL_ERROR;
	}
}


/**
* The physics code needs the actual mesh data.
* If the mesh is used for physics, we load the faces and verts
*
* @param bool update Update the existing arrya of `AssimpMesh`, v.s. creating anew.
**/
void AssimpModel::loadMeshdata(bool update)
{
	unsigned int n = 0;
	AssimpMesh *myMesh;

	for (; n < sc->mNumMeshes; ++n) {
		const struct aiMesh* mesh = sc->mMeshes[n];
		
		// Grab existing object or create new one
		if (update) {
			myMesh = this->meshes.at(n);
		} else {
			myMesh = new AssimpMesh();
			myMesh->numFaces = mesh->mNumFaces;
			this->meshes.push_back(myMesh);
		}
		
		// Copy face data
		myMesh->faces = new vector<AssimpFace>();
		myMesh->faces->reserve(mesh->mNumFaces);
		for (unsigned int t = 0; t < mesh->mNumFaces; ++t) {
			AssimpFace face;
			face.a = mesh->mFaces[t].mIndices[0];
			face.b = mesh->mFaces[t].mIndices[1];
			face.c = mesh->mFaces[t].mIndices[2];
			myMesh->faces->push_back(face);
		}
		
		// Copy verticies
		myMesh->verticies = new vector<glm::vec4>();
		myMesh->verticies->reserve(mesh->mNumVertices);
		for (unsigned int t = 0; t < mesh->mNumVertices; ++t) {
			myMesh->verticies->push_back(glm::vec4(mesh->mVertices[t].x, mesh->mVertices[t].y, mesh->mVertices[t].z, 1.0f));
		}
	}
}


/**
* Load materials.
* Only supports simple materials with a single texture at the moment.
**/
void AssimpModel::loadMaterials(Render3D* render)
{
	unsigned int n;
	aiString path;
	
	for (n = 0; n < sc->mNumMaterials; n++) {
		const aiMaterial* pMaterial = sc->mMaterials[n];
		AssimpMaterial *myMat = new AssimpMaterial();
		
		// Diffuse texture
		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				myMat->diffuse = this->loadTexture(render, path);
			}
		}
		
		// Normal map
		if (pMaterial->GetTextureCount(aiTextureType_NORMALS) > 0) {
			if (pMaterial->GetTexture(aiTextureType_NORMALS, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				myMat->normal = this->loadTexture(render, path);
			}
		}
		
		this->materials.push_back(myMat);
	}
}


/**
* Load a texture specified at a specified path.
*
* TODO: We should save these in a std::map so we don't load the same stuff multiple times.
**/
SpritePtr AssimpModel::loadTexture(Render3D* render, aiString path)
{
	std::string p(path.data);
	
	if (p.substr(0, 2) == ".\\") p = p.substr(2, p.size() - 2);
	if (p.substr(0, 2) == "./") p = p.substr(2, p.size() - 2);
	if (p.substr(0, 2) == "//") p = p.substr(2, p.size() - 2);
	
	return render->loadSprite("models/" + p, this->mod); 
}


/**
* Load the node tree
**/
void AssimpModel::loadNodes()
{
	this->rootNode = this->loadNode(this->sc->mRootNode, 0);
	assert(this->rootNode != NULL);
}


/**
* Load a node (and it's children) from the node tree
**/
AssimpNode* AssimpModel::loadNode(aiNode* nd, unsigned int depth)
{
	unsigned int i;
	AssimpNode* myNode = new AssimpNode();
	myNode->name = std::string(nd->mName.C_Str());
	
	aiMatrix4x4 m = nd->mTransformation;
	m.Transpose();
	myNode->transform = glm::make_mat4((float *) &m);
	
	//cout << std::string(depth*4, ' ') << myNode->name << "  " << myNode->transform[3][0] << "x" << myNode->transform[3][1] << "x" << myNode->transform[3][2] << endl;
	
	for (i = 0; i < nd->mNumMeshes; i++) {
		myNode->meshes.push_back(nd->mMeshes[i]);
		this->meshes[nd->mMeshes[i]]->nd = myNode;
	}
	
	for (i = 0; i < nd->mNumChildren; i++) {
		AssimpNode* child = loadNode(nd->mChildren[i], depth + 1);
		if (child != NULL) myNode->addChild(child);
	}
	
	return myNode;
}


/**
* Find a node by name
**/
AssimpNode* AssimpModel::findNode(string name)
{
	return this->findNode(this->rootNode, name);
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
	out->ticsPerSec = (float)anim->mTicksPerSecond;
	out->duration = (float)anim->mDuration;

	// Load animation channels
	out->anims.reserve(anim->mNumChannels);
	for (n = 0; n < anim->mNumChannels; n++) {
		const aiNodeAnim* pNodeAnim = anim->mChannels[n];

		AssimpNodeAnim* na = new AssimpNodeAnim();
		na->name = std::string(pNodeAnim->mNodeName.C_Str());

		// Positions
		na->position.reserve(pNodeAnim->mNumPositionKeys);
		for (m = 0; m < pNodeAnim->mNumPositionKeys; m++) {
			key.time = (float)pNodeAnim->mPositionKeys[m].mTime;
			key.vec = this->convVector(pNodeAnim->mPositionKeys[m].mValue);
			na->position.push_back(key);
		}

		// Rotations
		na->rotation.reserve(pNodeAnim->mNumRotationKeys);
		for (m = 0; m < pNodeAnim->mNumRotationKeys; m++) {
			key.time = (float)pNodeAnim->mRotationKeys[m].mTime;
			key.quat = this->convQuaternion(pNodeAnim->mRotationKeys[m].mValue);
			na->rotation.push_back(key);
		}

		// Scales
		na->scale.reserve(pNodeAnim->mNumScalingKeys);
		for (m = 0; m < pNodeAnim->mNumScalingKeys; m++) {
			key.time = (float)pNodeAnim->mScalingKeys[m].mTime;
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

	// Set to nothing
	for (unsigned int i = 0; i < (4 * mesh->mNumVertices); i++) {
		this->boneIds[i] = 0;
		this->boneWeights[i] = 0.0f;
	}

	// Keeps track of which index a given vertex is up to
	idx = (unsigned int*) malloc(sizeof(unsigned int) * mesh->mNumVertices);
	memset(idx, 0, sizeof(unsigned int) * mesh->mNumVertices);

	// Loop through the weights of all the bones
	// Save the id and the weight in the arrays as required
	for (n = 0; n < mesh->mNumBones; n++) {
		aiBone* bone = mesh->mBones[n];
		
		for (m = 0; m < bone->mNumWeights; m++) {
			aiVertexWeight w = bone->mWeights[m];

			if (w.mWeight < 0.01f) continue;
			if (idx[w.mVertexId] == MAX_WEIGHTS) continue;
			
			this->boneIds[w.mVertexId * 4 + idx[w.mVertexId]] = n;
			this->boneWeights[w.mVertexId * 4 + idx[w.mVertexId]] = w.mWeight;

			idx[w.mVertexId]++;
		}

		// We also have to save the bone data in our structure
		AssimpBone* bn = new AssimpBone();
		bn->name = std::string(bone->mName.C_Str());
		bn->offset = glm::make_mat4((float *) &(bone->mOffsetMatrix));
		
		aiMatrix4x4 m = bone->mOffsetMatrix;
		m.Transpose();
		bn->offset = glm::make_mat4((float *) &m);
		
		myMesh->bones.push_back(bn);
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


/**
* The AssimpBones have a pointer to their associated AssimpNode
* We need to hook these up
**/
void AssimpModel::setBoneNodes()
{
	for (vector<AssimpMesh*>::iterator it = this->meshes.begin(); it != this->meshes.end(); it++) {
		AssimpMesh* mesh = (*it);
		
		for (unsigned int i = 0; i < mesh->bones.size(); i++) {
			AssimpBone *bn = mesh->bones[i];
			bn->nd = this->findNode(this->rootNode, bn->name);
		}
	}
}



/**
* Create a collision shape object
**/
void AssimpModel::createCollisionShape()
{
	btVector3 sizeHE = this->getBoundingSizeHE();
	this->shape = new btBoxShape(sizeHE);
}


/**
* Return a physics representation of this model
**/
btCollisionShape* AssimpModel::getCollisionShape()
{
	if (this->shape == NULL) this->createCollisionShape();
	return this->shape;
}


