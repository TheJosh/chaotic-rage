// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include <map>
#include <glm/glm.hpp>
#include "../rage.h"
#include "assimpmodel.h"

using namespace std;


/**
* Represents no animation, just a single frame
**/
#define ANIMATION_NONE 255


class AnimPlay
{
	friend class RenderOpenGL;

	private:
		GameState* st;
		AssimpModel* model;
		AssimpAnimation* anim;
		map<AssimpNode*, glm::mat4> transforms;

		// just temporary(tm)
		unsigned int start_time;
		
	public:
		AnimPlay(AssimpModel* model);
		AnimPlay(AssimpModel* model, unsigned int animation);

		AssimpModel* getModel() { return this->model; }
		bool isAnimated() { return (this->anim != NULL); }

		void setAnimation(unsigned int animation);
		void calcTransformsStatic();
		void calcTransforms();

	private:
		void calcTransformNodeStatic(AssimpNode* nd, glm::mat4 transform);
		void calcTransformNode(AssimpNode* nd, glm::mat4 transform, float animTick);
		unsigned int findFrameTime(vector<AssimpAnimKey>* keys, float animTick);
		float mixFactor(vector<AssimpAnimKey>* keys, unsigned int index, float animTick);
		
};
