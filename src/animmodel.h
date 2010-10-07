// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


typedef int Mesh;
typedef int Texture;

class MeshFrame			// TODO: naming
{
	public:
		Mesh mesh;
		Texture tex;
		float px, py, pz;		// pos
		float rx, ry, rz;		// rotate
		float sx, sy, sz;		// scale
		
	public:
		MeshFrame();
};

class AnimModel
{
	public:
		// from data file
		string name;
		int num_frames;
		vector <MeshFrame *> meshframes;
		
		// dynamic
		int id;
		
	public:
		AnimModel();
};


vector<AnimModel*> * loadAllAnimModels(Mod * mod);
