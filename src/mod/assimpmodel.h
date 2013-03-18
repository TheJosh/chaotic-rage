// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include <assimp/scene.h>
#include "../rage.h"

using namespace std;


class AssimpMesh
{
	public:
		GLuint vao;
		int numFaces;
		int materialIndex;
};

class AssimpMaterial
{
	public:
		AssimpMaterial() : tex(NULL) {}
		
	public:
		SpritePtr tex;
};


class AssimpModel
{
	friend class RenderOpenGL;
	
	private:
		const struct aiScene* sc;
		btVector3 boundingSize;
		
	protected:
		Mod *mod;
		string name;
		vector<AssimpMesh*> meshes;
		vector<AssimpMaterial*> materials;
		
	public:
		AssimpModel(Mod* mod, string name);
		bool load(Render3D* render);
		
	public:
		const struct aiScene* getScene() { return this->sc; }
		btVector3 getBoundingSize();
		btVector3 getBoundingSizeHE();
		
	private:
		void calcBoundingSize();
		btVector3 calcBoundingSizeNode(const struct aiNode* nd);
};


