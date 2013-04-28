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
		AssimpModel* model;
		AssimpAnimation* anim;
		unsigned int animation;
		map<AssimpNode*, glm::mat4> transforms;

		// just temporary(tm)
		unsigned int frame;

	public:
		AnimPlay(AssimpModel* model);
		AnimPlay(AssimpModel* model, unsigned int animation);

		AssimpModel* getModel();
		bool isAnimated() { return this->animation != ANIMATION_NONE; }

		void setAnimation(unsigned int animation);
		void calcTransforms();

	private:
		void calcTransformNode(AssimpNode* nd, glm::mat4 transform);
};
