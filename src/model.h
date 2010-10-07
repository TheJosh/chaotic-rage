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

class ModelFrame
{
	public:
		Mesh mesh;
		Texture tex;
		int px, py, pz;		// pos
		int rx, ry, rz;		// rotate
		int sx, sy, sz;		// scale
		
	public:
		ModelFrame();
}

class Model
{
	public:
		// from data file
		vector <ModelFrame *> frames;
		
		// dynamic
		int id;
		
	public:
		Model();
};


vector<Model*> * loadAllModels(Mod * mod);
