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
	AssimpNodeAnim* anim = this->anim->anims[0];

	unsigned int positionKey = this->findFrameTime(&anim->position, animTick);
	unsigned int rotationKey = this->findFrameTime(&anim->rotation, animTick);
	unsigned int scaleKey = this->findFrameTime(&anim->scale, animTick);

	float positionMix = this->mixFactor(&anim->position, positionKey, animTick);
	float rotationMix = this->mixFactor(&anim->rotation, rotationKey, animTick);
	float scaleMix = this->mixFactor(&anim->scale, scaleKey, animTick);

	transform *= nd->transform;

	transform = glm::translate(transform, glm::mix(anim->position[positionKey].vec, anim->position[positionKey + 1].vec, positionMix));
	transform *= glm::toMat4(glm::mix(anim->rotation[rotationKey].quat, anim->rotation[rotationKey + 1].quat, rotationMix));
	transform = glm::scale(transform, glm::mix(anim->scale[scaleKey].vec, anim->scale[scaleKey + 1].vec, scaleMix));

	this->transforms[nd] = transform;

	for (vector<AssimpNode*>::iterator it = nd->children.begin(); it != nd->children.end(); it++) {
		calcTransformNode((*it), transform, animTick);
	}
}


/**
* Find a valid key from the list of keys, for a given time value (in ticks)
**/
unsigned int AnimPlay::findFrameTime(vector<AssimpAnimKey>* keys, float animTick)
{
	for (unsigned int i = 0; i < keys->size() - 1; i++) {
		if (animTick < keys->at(i + 1).time) {
			return i;
		}
	}
	
	assert(0);
}


/**
* Find a valid key from the list of keys, for a given time value (in ticks)
**/
float AnimPlay::mixFactor(vector<AssimpAnimKey>* keys, unsigned int index, float animTick)
{
	AssimpAnimKey* curr = &(keys->at(index));
	AssimpAnimKey* next = &(keys->at(index+1));
	
	return
		(animTick - curr->time)
		/
		(next->time - curr->time);
}


