// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include "../rage.h"

using namespace std;


/**
* Create an anim play
**/
AnimPlay::AnimPlay(AssimpModel* model)
	: model(model)
{
	this->setAnimation(ANIMATION_NONE);
}


/**
* Create an anim play, and set an animation
* If there isn't any animations, it will fall back to the static mesh
**/
AnimPlay::AnimPlay(AssimpModel* model, unsigned int animation)
	: model(model)
{
	this->setAnimation(animation);
}


/**
* Returns the model to render
**/
AssimpModel* AnimPlay::getModel()
{
	return this->model;
}


/**
* Set the current animation.
* If there isn't any animations, does nothing.
**/
void AnimPlay::setAnimation(unsigned int animation)
{
	this->animation = animation;

	if (this->model->animations.size() == 0) {
		this->animation = ANIMATION_NONE;
	}

	// If it's static, calculate once only
	if (this->animation == ANIMATION_NONE) {
		this->calcTransforms();
	}
}


/**
* Calculate the animation transforms for this AnimPlay.
*
* We have to calculate this per-AnimPlay (instead of per AssimpModel), otherwise
* animations wouldn't really work if you had two entities sharing an AssimpModel.
**/
void AnimPlay::calcTransforms()
{
	this->calcTransformNode(this->model->rootNode, glm::mat4());
}


/**
* Calculate the animation transforms for an assimp node and it's children
**/
void AnimPlay::calcTransformNode(AssimpNode* nd, glm::mat4 transform)
{
	transform *= nd->transform;

	this->transforms[nd] = transform;

	for (vector<AssimpNode*>::iterator it = nd->children.begin(); it != nd->children.end(); it++) {
		calcTransformNode((*it), transform);
	}
}
