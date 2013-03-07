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


class AssimpModel
{
	friend class RenderOpenGL;
	
	private:
		const struct aiScene* sc;
		Mod *mod;
		string name;
		
	protected:
		GLuint vao;
		int numFaces;
		
	public:
		AssimpModel(Mod* mod, string name);
		bool load();
		
	public:
		const struct aiScene* getScene() { return this->sc; }
};


