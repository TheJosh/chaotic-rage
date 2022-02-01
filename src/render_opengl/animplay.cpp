// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include "../rage.h"
#include "../game_state.h"
#include "../mod/mod.h"
#include "animplay.h"
#include "assimpmodel.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;


/**
* Create a static anim play
**/
AnimPlay::AnimPlay(AssimpModel* model)
	: model(model)
{
	this->st = model->mod->st;
	this->anim = NULL;
	this->start_time = 0;
	this->start_frame = 0;
	this->end_frame = 0;
	this->loop = true;
	this->ended_func = NULL;
	this->ended_data = NULL;
	this->pause_time = 0;
	this->custom_transform = glm::mat4(1.0f);
	this->calcTransformsStatic();
}


/**
* Set the current animation
* Also sets start and end frames, and looping options.
**/
void AnimPlay::setAnimation(unsigned int animation, unsigned int start_frame, unsigned int end_frame, bool loop)
{
	// No animations? Fall back to static
	if (this->model->animations.empty()) {
		this->clearAnimation();
		return;
	}

	// Set animation properties
	this->anim = this->model->animations[animation];
	this->start_time = st->game_time;
	this->start_frame = start_frame;
	this->end_frame = end_frame;
	this->loop = loop;
	this->pause_time = 0;
}


/**
* Set the current animation
* Also sets start and end frames
* Looping will be enabled
**/
void AnimPlay::setAnimation(unsigned int animation, unsigned int start_frame, unsigned int end_frame)
{
	this->setAnimation(animation, start_frame, end_frame, true);
}


/**
* Set the current animation
* Animation will be for all frames
**/
void AnimPlay::setAnimation(unsigned int animation)
{
	this->setAnimation(animation, 0, 0, true);
}


/**
* Set callback when animation ends.
* This only gets called for non-loop animations.
*
* After the animation has ended and the callback has been called,
* the animation is unset (but the transforms remain intact).
**/
void AnimPlay::setEndedCallback(AnimPlayCallback func, void* data)
{
	this->ended_func = func;
	this->ended_data = data;
}


/**
* Set callback when animation ends.
* Only sets the function, not the data
**/
void AnimPlay::setEndedCallback(AnimPlayCallback func)
{
	this->ended_func = func;
}


/**
* Clear the animation
**/
void AnimPlay::clearAnimation()
{
	this->anim = NULL;

	// If it's static, the calcs are really easy
	if (this->move_nodes.empty()) {
		this->calcTransformsStatic();
	}
}


/**
* Sets the "move node", a node which is transformed by some aspect of the game engine
* e.g. gun turrets
**/
void AnimPlay::addMoveNode(string node)
{
	AssimpNode* nd = this->model->findNode(this->model->rootNode, node);
	if (nd == NULL) return;

	this->move_nodes[nd] = glm::mat4();
}


/**
* Sets the "move node", a node which is transformed by some aspect of the game engine
* e.g. gun turrets
**/
void AnimPlay::addMoveNode(AssimpNode *nd)
{
	this->move_nodes[nd] = glm::mat4();
}


/**
* Remove a "move node", a node which is transformed by some aspect of the game engine
* If this brings the model back to static, we re-calc the transforms.
**/
void AnimPlay::removeMoveNode(AssimpNode *nd)
{
	this->move_nodes.erase(nd);

	// If it's static, the calcs are really easy
	if (this->anim == NULL && this->move_nodes.empty()) {
		this->calcTransformsStatic();
	}
}


/**
* Set the transform for a move node
**/
void AnimPlay::setMoveTransform(string node, glm::mat4 transform)
{
	AssimpNode* nd = this->model->findNode(this->model->rootNode, node);
	if (nd == NULL) return;

	this->move_nodes[nd] = transform;
}


/**
* Set the transform for a move node
**/
void AnimPlay::setMoveTransform(AssimpNode *nd, glm::mat4 transform)
{
	this->move_nodes[nd] = transform;
}


/**
* Set the custom transform
* This is applied to the root node
**/
void AnimPlay::setCustomTransform(glm::mat4 custom_transform)
{
	this->custom_transform = custom_transform;
	if (this->anim == NULL && this->move_nodes.empty()) {
		this->calcTransformsStatic();
	}
}


/**
* Set the custom transform to a translation matrix
**/
void AnimPlay::setCustomTransformTranslate(glm::vec3 translate)
{
	this->setCustomTransform(
		glm::translate(glm::mat4(1.0f), translate)
	);
}


/**
* Set the custom transform to a scale matrix
**/
void AnimPlay::setCustomTransformScale(glm::vec3 scale)
{
	this->setCustomTransform(
		glm::scale(glm::mat4(1.0f), scale)
	);
}


/**
* Pause the animation (goes hand-in-hand with resume() method)
**/
void AnimPlay::pause()
{
	this->pause_time = st->game_time;
}


/**
* Resume the animation (goes hand-in-hand with pause() method)
**/
void AnimPlay::resume()
{
	this->start_time += (st->game_time - this->pause_time);
	this->pause_time = 0;
}


/**
* Calculate the transforms, without any animation logic
* It's just a performance optimisation
**/
void AnimPlay::calcTransformsStatic()
{
	this->calcTransformNodeStatic(this->model->rootNode, this->custom_transform);
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

	for (vector<AssimpNode*>::iterator it = nd->children.begin(); it != nd->children.end(); ++it) {
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
	if (this->anim == NULL && this->move_nodes.empty()) return;

	float animTick;

	if (this->anim) {
		float timeSecs;

		if (this->pause_time == 0) {
			timeSecs = (st->game_time - this->start_time) / 1000.0f;
		} else {
			timeSecs = (this->pause_time - this->start_time) / 1000.0f;
		}

		float ticsPerSec = this->anim->ticsPerSec != 0.0 ? this->anim->ticsPerSec : 25.0f;

		// If a start/end frame has been specified, we assume a fixed frame length
		if (this->start_frame != 0 && this->end_frame != 0) {
			float frameTime = this->anim->frameTime;
			float totalTime = frameTime * (this->end_frame - this->start_frame);

			if (this->loop) {
				animTick = fmod(timeSecs * ticsPerSec, totalTime);
			} else {
				animTick = timeSecs * ticsPerSec;
				if (animTick > totalTime) {
					animTick = totalTime;
					if (this->ended_func) { (*this->ended_func)(this, this->ended_data); }
					this->anim = NULL;
				}
			}

			animTick += (frameTime * this->start_frame);

		} else if (this->loop) {
			animTick = fmod(timeSecs * ticsPerSec, this->anim->duration);

		} else {
			animTick = timeSecs * ticsPerSec;
			if (animTick > this->anim->duration) {
				animTick = this->anim->duration;
				if (this->ended_func) { (*this->ended_func)(this, this->ended_data); }
				this->anim = NULL;
			}
		}

	} else {
		animTick = 0.0f;
	}

	this->calcTransformNode(this->model->rootNode, this->custom_transform, animTick);
}


/**
* Calculate the animation transforms for an assimp node and it's children
*
* @param AssimpNode* nd The node to calculate the tranform for
* @param glm::mat4 transform The transform of the parent node
* @param unsigned int time The current animation time
**/
void AnimPlay::calcTransformNode(AssimpNode* nd, glm::mat4 &transform, float animTick)
{
	glm::mat4 local;

	if (this->move_nodes.find(nd) != this->move_nodes.end()) {
		local = nd->transform * this->move_nodes[nd];

	} else if (this->anim != NULL) {
		std::map<std::string, AssimpNodeAnim*>::iterator it = this->anim->anims.find(nd->name);

		// If channel found, do animation
		if (it != this->anim->anims.end()) {
			AssimpNodeAnim* animNode = it->second;
			
			unsigned int positionKey = this->findFrameTime(&animNode->position, animTick);
			unsigned int rotationKey = this->findFrameTime(&animNode->rotation, animTick);
			unsigned int scaleKey = this->findFrameTime(&animNode->scale, animTick);

			// The below supports animations which have keyframes only instead
			// of every frame specified.
			/*
			float positionMix = this->mixFactor(&animNode->position, positionKey, animTick);
			float rotationMix = this->mixFactor(&animNode->rotation, rotationKey, animTick);
			float scaleMix = this->mixFactor(&animNode->scale, scaleKey, animTick);
			
			glm::mat4 trans = glm::translate(glm::mat4(), glm::mix(animNode->position[positionKey].vec, animNode->position[positionKey + 1].vec, positionMix));
			glm::mat4 rot = glm::toMat4(glm::mix(animNode->rotation[rotationKey].quat, animNode->rotation[rotationKey + 1].quat, rotationMix));
			glm::mat4 scale = glm::scale(glm::mat4(), glm::mix(animNode->scale[scaleKey].vec, animNode->scale[scaleKey + 1].vec, scaleMix));
			*/

			local = animNode->position[positionKey].mat
				* animNode->rotation[rotationKey].mat
				* animNode->scale[scaleKey].mat;
		} else {
			local = nd->transform;
		}

	} else {
		local = nd->transform;
	}

	glm::mat4 localTransform = transform * local;
	this->transforms[nd] = localTransform;

	for (vector<AssimpNode*>::iterator it = nd->children.begin(); it != nd->children.end(); ++it) {
		calcTransformNode((*it), localTransform, animTick);
	}
}


/**
* Apply animation transforms into the "bone transforms" ready for sending to OpenGL
**/
void AnimPlay::applyBoneTransforms(AssimpMesh* mesh)
{
	if (this->anim == NULL && this->move_nodes.empty() && !bone_transforms.empty()) return;

	// set all bones to empty transform
	bone_transforms.clear();
	bone_transforms.reserve(MAX_BONES);
	for (unsigned int i = 0; i < MAX_BONES; i++) {
		bone_transforms.push_back(glm::mat4(1.0f));
	}

	// Iterate through the bones and set the transforms from the nodes
	for (unsigned int i = 0; i < mesh->bones.size(); i++) {
		AssimpBone *bn = mesh->bones[i];
		if (bn->nd == NULL) continue;

		map<AssimpNode*, glm::mat4>::iterator it = this->transforms.find(bn->nd);
		if (it == this->transforms.end()) continue;

		this->bone_transforms[i] = it->second * bn->offset;
	}
}


/**
* Get the transform for a given node
**/
glm::mat4 AnimPlay::getNodeTransform(AssimpNode* nd)
{
	std::map<AssimpNode*, glm::mat4>::iterator local = this->transforms.find(nd);
	assert(local != this->transforms.end());
	return local->second;
}


/**
* Find a valid key from the list of keys, for a given time value (in ticks)
*
* @return int Frame index 0-based
**/
unsigned int AnimPlay::findFrameTime(vector<AssimpAnimKey>* keys, float animTick)
{
	unsigned int i = 0;

	vector<AssimpAnimKey>::iterator begin = keys->begin() + 1;
	vector<AssimpAnimKey>::iterator end = keys->end();

	for(vector<AssimpAnimKey>::iterator it = begin; it != end; ++it, ++i) {
		if (animTick < (*it).time) {
			return i;
		}
	}

	assert(0);
	return 0;
}


/**
* Find a valid key from the list of keys, for a given time value (in ticks)
**/
float AnimPlay::mixFactor(vector<AssimpAnimKey>* keys, unsigned int index, float animTick)
{
	AssimpAnimKey* curr = &(*keys)[index];
	AssimpAnimKey* next = curr + 1;
	
	return
		(animTick - curr->time)
		/
		(next->time - curr->time);
}
