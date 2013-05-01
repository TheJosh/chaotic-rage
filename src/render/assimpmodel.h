// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include <assimp/scene.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "../rage.h"

using namespace std;


/**
* How many bones can be in an model
**/
#define MAX_BONES 32

/**
* How many bone ids/weights can be assigned to a vertex
**/
#define MAX_WEIGHTS 4



class AssimpBone
{
	public:
		string name;
		glm::mat4 offset;
};

class AssimpMesh
{
	public:
		GLuint vao;
		int numFaces;
		int materialIndex;
		vector<AssimpBone*> bones; 
};

class AssimpMaterial
{
	public:
		AssimpMaterial() : tex(NULL) {}
		
	public:
		SpritePtr tex;
};

class AssimpNode
{
	public:
		AssimpNode* parent;
		vector<AssimpNode*> children;
		vector<unsigned int> meshes;
		glm::mat4 transform;
		string name;

	public:
		AssimpNode() : parent(NULL) {}
		
		void addChild(AssimpNode* child) {
			this->children.push_back(child);
			child->parent = this;
		}
};

class AssimpAnimKey
{
	public:
		glm::vec3 vec;		// vector for position/scale
		glm::quat quat;		// quaternion for rotation
		float time;
};

class AssimpNodeAnim
{
	public:
		string name;
		vector<AssimpAnimKey> position;
		vector<AssimpAnimKey> rotation;
		vector<AssimpAnimKey> scale;
};

class AssimpAnimation
{
	public:
		string name;
		float duration;
		float ticsPerSec;
		vector<AssimpNodeAnim*> anims;
};


class AssimpModel
{
	friend class RenderOpenGL;
	friend class AnimPlay;

	private:
		const struct aiScene* sc;			// TODO: remove this, pass it as a func. arg everywhere instead.
		unsigned int* boneIds;
		float* boneWeights;
		btVector3 boundingSize;
		
	protected:
		Mod *mod;
		string name;
		vector<AssimpMesh*> meshes;
		vector<AssimpMaterial*> materials;
		vector<AssimpAnimation*> animations;
		AssimpNode* rootNode;
		
	public:
		AssimpModel(Mod* mod, string name);
		bool load(Render3D* render);
		
	public:
		const struct aiScene* getScene() { return this->sc; }		// TODO: remove this too
		btVector3 getBoundingSize();
		btVector3 getBoundingSizeHE();
		
	private:
		void calcBoundingSize();
		btVector3 calcBoundingSizeNode(const struct aiNode* nd, aiMatrix4x4* trafo);

		void loadNodes();
		AssimpNode* loadNode(aiNode* nd);
		AssimpNode* findNode(AssimpNode* nd, string name);

		void loadAnimations();
		AssimpAnimation* loadAnimation(const aiAnimation* anim);

		glm::vec3 convVector(aiVector3D in);
		glm::quat convQuaternion(aiQuaternion in);

		void loadBones(const aiMesh* mesh, AssimpMesh* myMesh);
		unsigned int* getBoneIds();
		float* getBoneWeights();
		void freeBones();
};


