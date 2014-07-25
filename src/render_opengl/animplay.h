// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <map>
#include <glm/glm.hpp>
#include "../rage.h"
#include "assimpmodel.h"

using namespace std;


/**
* Represents no animation, just a single frame
**/
#define ANIMATION_NONE 255


/**
* Function signature for callbacks when animations are finished
**/
typedef void (*AnimPlayCallback) (AnimPlay* play, void* data);


/**
* Stores info about a single animation which is currently running
* Also used for static meshes.
**/
class AnimPlay
{
	friend class RenderOpenGL;

	private:
		GameState* st;
		AssimpModel* model;

		// Currently running animation
		AssimpAnimation* anim;
		unsigned int start_time;
		unsigned int start_frame;
		unsigned int end_frame;
		bool loop;
		AnimPlayCallback ended_func;
		void* ended_data;
		unsigned int pause_time;

		// Move nodes and their transforms
		map<AssimpNode*, glm::mat4> move_nodes;

		// These are calculated by calcTransforms()
		// and then fed into the renderer
		map<AssimpNode*, glm::mat4> transforms;

		// Calculated bone transforms
		vector<glm::mat4> bone_transforms;

	public:
		AnimPlay(AssimpModel* model);

		void setAnimation(unsigned int animation, unsigned int start_frame, unsigned int end_frame, bool loop);
		void setAnimation(unsigned int animation, unsigned int start_frame, unsigned int end_frame);
		void setAnimation(unsigned int animation);
		void clearAnimation();

		void setEndedCallback(AnimPlayCallback func, void* data);
		void setEndedCallback(AnimPlayCallback func);
		void setEndedCallback(void* data);

		void addMoveNode(string node);
		void addMoveNode(AssimpNode *nd);
		void setMoveTransform(string node, glm::mat4 transform);
		void setMoveTransform(AssimpNode *nd, glm::mat4 transform);
		void resetMoveTransform(string node);

		AssimpModel* getModel() { return this->model; }

		void calcTransformsStatic();
		void calcTransforms();
		void calcBoneTransforms();

		glm::mat4 getNodeTransform(AssimpNode* nd);

		void pause();
		void resume();

	private:
		void calcTransformNodeStatic(AssimpNode* nd, glm::mat4 transform);
		void calcTransformNode(AssimpNode* nd, glm::mat4 transform, float animTick);
		unsigned int findFrameTime(vector<AssimpAnimKey>* keys, float animTick);
		float mixFactor(vector<AssimpAnimKey>* keys, unsigned int index, float animTick);

};
