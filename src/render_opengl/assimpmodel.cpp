// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "assimpmodel.h"

#include <iostream>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <btBulletDynamicsCommon.h>

#include "gl_debug.h"
#include "../mod/mod.h"
#include "render_opengl.h"
#include "../render/sprite.h"

using namespace std;


/**
* Create a model. It's not loaded yet, use load() to load it.
**/
AssimpModel::AssimpModel(Mod* mod, string name)
{
	this->mod = mod;
	this->name = name;
	this->shape = NULL;
	this->boneIds = NULL;
	this->boneWeights = NULL;
	this->rootNode = NULL;
	this->recenter = true;
}


/**
* Free loaded data
**/
AssimpModel::~AssimpModel()
{
	delete this->shape;
	delete this->rootNode;
	delete this->boneIds;
	delete this->boneWeights;
}


/**
* Load the model.
* Return true on success, false on failure.
**/
bool AssimpModel::load(Render3D* render, bool meshdata, AssimpLoadFlags flags)
{
	Assimp::Importer importer;

	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
	importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_COLORS | aiComponent_LIGHTS | aiComponent_CAMERAS);
	importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, 65535);
	importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, 65535);
	importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, MAX_WEIGHTS);
	importer.SetPropertyInteger(AI_CONFIG_IMPORT_COLLADA_USE_COLLADA_NAMES, 1);

	unsigned int assflags = aiProcess_CalcTangentSpace
		| aiProcess_Triangulate
		| aiProcess_JoinIdenticalVertices
		| aiProcess_SortByPType
		| aiProcess_FlipUVs
		| aiProcess_FindDegenerates
		| aiProcess_ImproveCacheLocality
		| aiProcess_RemoveComponent
		| aiProcess_LimitBoneWeights;

	// For map meshes we flatten geometry
	if (flags & ALM_FlattenGeometry) {
		assflags |= aiProcess_PreTransformVertices;
		assflags |= aiProcess_RemoveRedundantMaterials;
		assflags |= aiProcess_OptimizeMeshes;
		assflags |= aiProcess_FindInvalidData;
		assflags |= aiProcess_SplitLargeMeshes;
	}

	// Normally models are re-centered; this isn't always deisred.
	if (flags & ALF_NoRecenter) {
		this->recenter = false;
	}

	// Read the file from the mod
	Sint64 len;
	Uint8 * data = this->mod->loadBinary("models/" + this->name, &len);
	if (! data) {
		this->mod->setLoadErr("Failed to load %s; file read failed", this->name.c_str());
		return false;
	}

	// Check we aren't larger than size_t
	if (len > MAX_FILE_SIZE) {
		this->mod->setLoadErr("Failed to load %s; file too large", this->name.c_str());
		return false;
	}

	// Do the import
	const struct aiScene* sc = importer.ReadFileFromMemory((const void*) data, (size_t)len, assflags, this->name.c_str());
	if (!sc) {
		this->mod->setLoadErr("Failed to load %s; file read failed; %s", this->name.c_str(), importer.GetErrorString());
		return false;
	}

	free(data);

	if (debug_enabled("loadbones")) {
		cout << endl << this->name << endl;
	}

	if (render != NULL && render->is3D()) {
		this->loadMeshes(true, sc);
		this->loadMaterials(render, sc);
	} else {
		this->loadMeshes(false, sc);
	}

	if (meshdata) {
		this->loadMeshdata(render != NULL, sc);
	}

	this->loadNodes(sc);
	this->loadAnimations(sc);
	this->calcBoundingBox(sc);
	this->setBoneNodes();

	return true;
}


/**
* Returns the bounding size of the mesh of the first frame
**/
void AssimpModel::calcBoundingBox(const struct aiScene* sc)
{
	aiMatrix4x4 trafo;
	aiIdentityMatrix4(&trafo);
	aiVector3D min, max;

	// Calculate bounds and size
	min.x = min.y = min.z = 1e10f;
	max.x = max.y = max.z = -1e10f;
	this->calcBoundingBoxNode(sc->mRootNode, &min, &max, &trafo, sc);
	boundingSize = btVector3(max.x - min.x, max.y - min.y, max.z - min.z);

	// Calculate recenter offset
	recenterOffs = glm::vec4(
		(min.x + max.x) / -2.0f,
		(min.y + max.y) / -2.0f,
		(min.z + max.z) / -2.0f,
		1.0f
	);

	// Recenter the model to the middle of the bounding box
	if (this->recenter) {
		this->rootNode->transform[3] = recenterOffs;
		if (debug_enabled("loadbones")) {
			cout << "Recenter '" << rootNode->name << "' to " << rootNode->transform[3][0] << "x" << rootNode->transform[3][1] << "x" << rootNode->transform[3][2] << endl;
		}
	}
}


/**
* Returns the bounding size of the mesh of the first frame
**/
void AssimpModel::calcBoundingBoxNode(const aiNode* nd, aiVector3D* min, aiVector3D* max, aiMatrix4x4* trafo, const struct aiScene* sc)
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
		this->calcBoundingBoxNode(nd->mChildren[n], min, max, trafo, sc);
	}

	*trafo = prev;
}


/**
* Load a model into a VAO, VBOs etc.
**/
void AssimpModel::loadMeshes(bool opengl, const struct aiScene* sc)
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

		assert(mesh->mNumFaces * 3 < 65535);

		if (debug_enabled("loadmesh")) {
			cout << this->name << ":" << n;
			cout << "; " << mesh->mNumFaces << " faces";
			cout << "; " << mesh->mNumVertices << " vertices";
			cout << endl;
		}

		// VAO
		myMesh->vao = new GLVAO();

		// Prep face array for VBO
		Uint16 *faceArray;
		faceArray = (Uint16 *)malloc(sizeof(Uint16) * mesh->mNumFaces * 3);
		assert(faceArray);

		// Copy face data
		unsigned int faceIndex = 0;
		for (unsigned int t = 0; t < mesh->mNumFaces; ++t) {
			const struct aiFace* face = &mesh->mFaces[t];
			faceArray[faceIndex++] = face->mIndices[0];
			faceArray[faceIndex++] = face->mIndices[1];
			faceArray[faceIndex++] = face->mIndices[2];
		}

		// Faces
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Uint16) * mesh->mNumFaces * 3, faceArray, GL_STATIC_DRAW);
		myMesh->vao->setIndex(buffer);
		if (debug_enabled("loadmesh")) cout << string(4, ' ') << "Index -> #" << buffer << endl;
		free(faceArray);

		// Positions
		if (mesh->HasPositions()) {
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*mesh->mNumVertices, mesh->mVertices, GL_STATIC_DRAW);
			myMesh->vao->setPosition(buffer);
			if (debug_enabled("loadmesh")) cout << string(4, ' ') << "Positions -> #" << buffer << endl;
		}

		// Normals
		if (mesh->HasNormals()) {
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*mesh->mNumVertices, mesh->mNormals, GL_STATIC_DRAW);
			myMesh->vao->setNormal(buffer);
			if (debug_enabled("loadmesh")) cout << string(4, ' ') << "Normals -> #" << buffer << endl;
		}

		// UVs
		for (unsigned int i = 0; i < UV_CHANNELS; ++i) {
			if (mesh->HasTextureCoords(i)) {
				glGenBuffers(1, &buffer);
				glBindBuffer(GL_ARRAY_BUFFER, buffer);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*mesh->mNumVertices, mesh->mTextureCoords[i], GL_STATIC_DRAW);
				myMesh->vao->setTexUV(buffer, i);
				if (debug_enabled("loadmesh")) cout << string(4, ' ') << "UVs " << i << " -> #" << buffer << endl;
			}
		}

		// Bone IDs and Weights
		if (mesh->HasBones()) {
			this->loadBones(mesh, myMesh);
			this->recenter = false;

			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Uint8)*4*mesh->mNumVertices, this->boneIds, GL_STATIC_DRAW);
			myMesh->vao->setBoneId(buffer);
			if (debug_enabled("loadmesh")) cout << string(4, ' ') << "Bone IDs -> #" << buffer << endl;

			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*mesh->mNumVertices, this->boneWeights, GL_STATIC_DRAW);
			myMesh->vao->setBoneWeight(buffer);
			if (debug_enabled("loadmesh")) cout << string(4, ' ') << "Bone weights -> #" << buffer << endl;

			this->freeBones();
		}

		// Tangents and Bittangents, for normal mapping
		// TODO: Determine if there actually is a normal map in use on this mesh!
		if (mesh->HasTangentsAndBitangents()) {
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*mesh->mNumVertices, mesh->mTangents, GL_STATIC_DRAW);
			myMesh->vao->setTangent(buffer);
			if (debug_enabled("loadmesh")) cout << string(4, ' ') << "Tangents -> #" << buffer << endl;

			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*mesh->mNumVertices, mesh->mBitangents, GL_STATIC_DRAW);
			myMesh->vao->setBitangent(buffer);
			if (debug_enabled("loadmesh")) cout << string(4, ' ') << "Bitangents -> #" << buffer << endl;
		}

		myMesh->vao->unbind();

		CHECK_OPENGL_ERROR;
	}
}


/**
* The physics code needs the actual mesh data.
* If the mesh is used for physics, we load the faces and verts
*
* @param bool update Update the existing array of `AssimpMesh`, v.s. creating a new.
**/
void AssimpModel::loadMeshdata(bool update, const struct aiScene* sc)
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
void AssimpModel::loadMaterials(Render3D* render, const struct aiScene* sc)
{
	unsigned int n;
	aiString path;

	for (n = 0; n < sc->mNumMaterials; n++) {
		const aiMaterial* pMaterial = sc->mMaterials[n];
		AssimpMaterial *myMat = new AssimpMaterial();

		// Diffuse texture
		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				myMat->diffuse = this->loadTexture(render, &path, sc);
			}
		}

		// Normal map
		if (pMaterial->GetTextureCount(aiTextureType_NORMALS) > 0) {
			if (pMaterial->GetTexture(aiTextureType_NORMALS, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				myMat->normal = this->loadTexture(render, &path, sc);
			}
		}

		// Lightmap (e.g. Ambient Occlusion)
		if (pMaterial->GetTextureCount(aiTextureType_LIGHTMAP) > 0) {
			if (pMaterial->GetTexture(aiTextureType_LIGHTMAP, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				myMat->lightmap = this->loadTexture(render, &path, sc);
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
SpritePtr AssimpModel::loadTexture(Render3D* render, aiString* path, const struct aiScene* sc)
{
	string p(path->data);

	// Check if it's an embedded texture
	const aiTexture* tex = sc->GetEmbeddedTexture(path->data);
	if (tex != NULL) {
		return this->loadEmbeddedTexture(render, tex);
	}

	// Cleanup filename path
	if (p.substr(0, 2) == ".\\") p = p.substr(2, p.size() - 2);
	if (p.substr(0, 2) == "./") p = p.substr(2, p.size() - 2);
	if (p.substr(0, 2) == "//") p = p.substr(2, p.size() - 2);

	return render->loadSprite("models/" + p, this->mod);
}


/**
* Load an embedded texture into the 3D renderer
**/
SpritePtr AssimpModel::loadEmbeddedTexture(Render3D* render, const aiTexture* tex)
{
	SpritePtr sprite;

	if (tex->mHeight == 0) {
		// Compressed data loaded using IMG_Load
		SDL_RWops* rw = SDL_RWFromConstMem(tex->pcData, tex->mWidth);
		sprite = render->loadSpriteFromRWops(rw, string(tex->mFilename.data));
		SDL_RWclose(rw);
	} else {
		// Uncompressed data loaded directly into OpenGL
		sprite = new Sprite();
		sprite->w = tex->mWidth;
		sprite->h = tex->mHeight;
		sprite->orig = NULL;
		sprite->pixels = render->loadTextureRBGA(tex->pcData, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, tex->mWidth, tex->mHeight);
	}

	return sprite;
}


/**
* Load the node tree
**/
void AssimpModel::loadNodes(const struct aiScene* sc)
{
	assert(sc != NULL);
	this->rootNode = this->loadNode(sc->mRootNode, 0);
	assert(this->rootNode != NULL);
}


/**
* Load a node (and it's children) from the node tree
**/
AssimpNode* AssimpModel::loadNode(aiNode* nd, unsigned int depth)
{
	unsigned int i;
	AssimpNode* myNode = new AssimpNode();
	myNode->name = string(nd->mName.C_Str());

	aiMatrix4x4 m = nd->mTransformation;
	m.Transpose();
	myNode->transform = glm::make_mat4((float *) &m);

	if (debug_enabled("loadbones")) {
		cout << string(depth*4, ' ') << myNode->name << "  " << myNode->transform[3][0] << "x" << myNode->transform[3][1] << "x" << myNode->transform[3][2] << endl;
	}

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

	for (vector<AssimpNode*>::iterator it = nd->children.begin(); it != nd->children.end(); ++it) {
		AssimpNode* maybe = this->findNode((*it), name);
		if (maybe) return maybe;
	}

	return NULL;
}


/**
* Load the animations for this assimp model
**/
void AssimpModel::loadAnimations(const struct aiScene* sc)
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
	out->name = string(anim->mName.C_Str());
	out->ticsPerSec = (float)anim->mTicksPerSecond;
	out->duration = (float)anim->mDuration;

	// Calculate the length of a single frame
	out->frameTime = (float)anim->mChannels[0]->mPositionKeys[1].mTime
		- (float)anim->mChannels[0]->mPositionKeys[0].mTime;

	// Load animation channels
	for (n = 0; n < anim->mNumChannels; n++) {
		const aiNodeAnim* pNodeAnim = anim->mChannels[n];

		AssimpNodeAnim* na = new AssimpNodeAnim();
		na->name = string(pNodeAnim->mNodeName.C_Str());

		// Positions
		na->position.reserve(pNodeAnim->mNumPositionKeys);
		for (m = 0; m < pNodeAnim->mNumPositionKeys; m++) {
			key.time = (float)pNodeAnim->mPositionKeys[m].mTime;
			key.vec = this->convVector(pNodeAnim->mPositionKeys[m].mValue);
			key.mat = glm::translate(glm::mat4(), key.vec);
			na->position.push_back(key);
		}

		// Rotations
		na->rotation.reserve(pNodeAnim->mNumRotationKeys);
		for (m = 0; m < pNodeAnim->mNumRotationKeys; m++) {
			key.time = (float)pNodeAnim->mRotationKeys[m].mTime;
			key.quat = this->convQuaternion(pNodeAnim->mRotationKeys[m].mValue);
			key.mat = glm::toMat4(key.quat);
			na->rotation.push_back(key);
		}

		// Scales
		na->scale.reserve(pNodeAnim->mNumScalingKeys);
		for (m = 0; m < pNodeAnim->mNumScalingKeys; m++) {
			key.time = (float)pNodeAnim->mScalingKeys[m].mTime;
			key.vec = this->convVector(pNodeAnim->mScalingKeys[m].mValue);
			key.mat = glm::scale(glm::mat4(), key.vec);
			na->scale.push_back(key);
		}

		out->anims.insert(
			std::pair<std::string, AssimpNodeAnim*>(na->name, na)
		);
	}

	return out;
}


/**
* Returns the bounding size of the mesh of the first frame
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
	unsigned int m;
	Uint8 n;
	unsigned int *idx;

	// Ensure mesh is valid
	assert(mesh->mNumBones < MAX_BONES);

	// Allocate space for the IDs and weights
	this->boneIds = (Uint8*) malloc(sizeof(Uint8) * 4 * mesh->mNumVertices);
	this->boneWeights = (float*) malloc(sizeof(float) * 4 * mesh->mNumVertices);

	// Set to nothing
	for (m = 0; m < (4 * mesh->mNumVertices); m++) {
		this->boneIds[m] = 0;
		this->boneWeights[m] = 0.0f;
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
		bn->name = string(bone->mName.C_Str());

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
Uint8* AssimpModel::getBoneIds()
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
* Free the bone data
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
	for (vector<AssimpMesh*>::iterator it = this->meshes.begin(); it != this->meshes.end(); ++it) {
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
