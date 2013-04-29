// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include "../rage.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace std;


/**
* Create an anim play
**/
AnimPlay::AnimPlay(AssimpModel* model)
	: model(model)
{
	this->st = model->mod->st;
	this->setAnimation(ANIMATION_NONE);
}


/**
* Create an anim play, and set an animation
* If there isn't any animations, it will fall back to the static mesh
**/
AnimPlay::AnimPlay(AssimpModel* model, unsigned int animation)
	: model(model)
{
	this->st = model->mod->st;
	this->setAnimation(animation);
}


/**
* Set the current animation.
* If there isn't any animations, does nothing.
**/
void AnimPlay::setAnimation(unsigned int animation)
{
	if (this->model->animations.size() == 0) {
		animation = ANIMATION_NONE;
	}

	// If it's static, calculate transforms once only
	if (animation == ANIMATION_NONE) {
		this->anim = NULL;
		this->calcTransformsStatic();
		
	} else {
		this->anim = this->model->animations[animation];
		this->start_time = st->game_time;
	}
}


/**
* Calculate the transforms, without any animation logic
* It's just a performance optimisation
**/
void AnimPlay::calcTransformsStatic()
{
	this->calcTransformNodeStatic(this->model->rootNode, glm::mat4());
}


/**
* Calculate the animation transforms for an assimp node and it's children
*
* @param AssimpNode* nd The node to calculate the tranform for
* @param glm::mat4 transform The transform of the parent node
**/
void AnimPlay::calcTransformNodeStatic(AssimpNode* nd, glm::mat4 transform)
{
	transform *= nd->transform;
	
	this->transforms[nd] = transform;
	
	for (vector<AssimpNode*>::iterator it = nd->children.begin(); it != nd->children.end(); it++) {
		calcTransformNodeStatic((*it), transform);
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
	float timeSecs = (st->game_time - this->start_time) / 1000.0f;
	float ticsPerSec = this->anim->ticsPerSec != 0.0 ? this->anim->ticsPerSec : 25.0f;
	float animTick = fmod(timeSecs * ticsPerSec, this->anim->duration);
	
	this->calcTransformNode(this->model->rootNode, glm::mat4(), animTick);
}


/**
* Calculate the animation transforms for an assimp node and it's children
*
* @param AssimpNode* nd The node to calculate the tranform for
* @param glm::mat4 transform The transform of the parent node
* @param unsigned int time The current animation time
**/
void AnimPlay::calcTransformNode(AssimpNode* nd, glm::mat4 transform, float animTick)
{
	AssimpAnimKey* scaleKey = this->findFrameTime(&this->anim->anims[0]->scale, animTick);
	AssimpAnimKey* rotationKey = this->findFrameTime(&this->anim->anims[0]->rotation, animTick);
	AssimpAnimKey* positionKey = this->findFrameTime(&this->anim->anims[0]->position, animTick);
	
	transform *= nd->transform;
	
	transform = glm::translate(transform, positionKey->vec);
	transform *= glm::toMat4(rotationKey->quat);
	transform = glm::scale(transform, scaleKey->vec);
	
	

	this->transforms[nd] = transform;

	for (vector<AssimpNode*>::iterator it = nd->children.begin(); it != nd->children.end(); it++) {
		calcTransformNode((*it), transform, animTick);
	}
}


/**
* Find a valid key from the list of keys, for a given time value (in ticks)
**/
AssimpAnimKey* AnimPlay::findFrameTime(vector<AssimpAnimKey>* keys, float animTick)
{
	for (vector<AssimpAnimKey>::iterator it = keys->begin(); it != keys->end(); it++) {
		if (animTick < (*it).time) {
			return &(*it);
		}
	}
	
	assert(0);
}


