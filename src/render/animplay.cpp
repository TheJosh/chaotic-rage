// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include "../rage.h"
#include "_render.h"

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
	this->move = NULL;
	this->anim = NULL;
	this->calcTransformsStatic();
}


/**
* Create an anim play, and set an animation
* If there isn't any animations, it will fall back to the static mesh
**/
AnimPlay::AnimPlay(AssimpModel* model, unsigned int animation)
	: model(model)
{
	this->st = model->mod->st;
	this->move = NULL;
	this->setAnimation(animation);
}


/**
* Is this animPlay animated (true) or static (false)
* Note that "animated" is also the case for move nodes
**/
bool AnimPlay::isAnimated()
{
	return (this->anim != NULL || this->move != NULL);
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

	// Grab the anim info
	if (animation == ANIMATION_NONE) {
		this->clearAnimation();
	} else {
		this->anim = this->model->animations[animation];
		this->start_time = st->game_time;
	}
}


/**
* Clear the animation
**/
void AnimPlay::clearAnimation()
{
	this->anim = NULL;

	// If it's static, the calcs are really easy
	if (this->anim == NULL && this->move == NULL) {
		this->calcTransformsStatic();
	}
}


/**
* Sets the "move node", a node which is transformed by some aspect of the game engine
* e.g. gun turrets
**/
void AnimPlay::setMoveNode(string node)
{
	this->move = this->model->findNode(this->model->rootNode, node);
	if (this->move == NULL) {
		this->clearMoveNode();
	}
	this->move_transform = glm::mat4();
}


/**
* Clears the "move node", a node which is transformed by some aspect of the game engine.
* Because this might bring the model back to static, we re-calc the transforms.
**/
void AnimPlay::clearMoveNode()
{
	this->move = NULL;

	// If it's static, the calcs are really easy
	if (this->anim == NULL && this->move == NULL) {
		this->calcTransformsStatic();
	}
}


/**
* Set the transform for the move node
**/
void AnimPlay::setMoveTransform(glm::mat4 transform)
{
	this->move_transform = transform;
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
	float animTick;
	
	if (this->anim) {
		float timeSecs = (st->game_time - this->start_time) / 1000.0f;
		float ticsPerSec = this->anim->ticsPerSec != 0.0 ? this->anim->ticsPerSec : 25.0f;
		animTick = fmod(timeSecs * ticsPerSec, this->anim->duration);
	} else {
		animTick = 0.0f;
	}
	
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
	glm::mat4 local;

	if (this->move && this->move == nd) {
		local = nd->transform * this->move_transform;

	} else if (this->anim && this->anim->anims[0]->name == nd->name) {
		AssimpNodeAnim* animNode = this->anim->anims[0];

		unsigned int positionKey = this->findFrameTime(&animNode->position, animTick);
		unsigned int rotationKey = this->findFrameTime(&animNode->rotation, animTick);
		unsigned int scaleKey = this->findFrameTime(&animNode->scale, animTick);

		float positionMix = this->mixFactor(&animNode->position, positionKey, animTick);
		float rotationMix = this->mixFactor(&animNode->rotation, rotationKey, animTick);
		float scaleMix = this->mixFactor(&animNode->scale, scaleKey, animTick);

		glm::mat4 trans = glm::translate(glm::mat4(), glm::mix(animNode->position[positionKey].vec, animNode->position[positionKey + 1].vec, positionMix));
		glm::mat4 rot = glm::toMat4(glm::mix(animNode->rotation[rotationKey].quat, animNode->rotation[rotationKey + 1].quat, rotationMix));
		glm::mat4 scale = glm::scale(glm::mat4(), glm::mix(animNode->scale[scaleKey].vec, animNode->scale[scaleKey + 1].vec, scaleMix));
	
		local = trans * rot * scale;

	} else {
		local = nd->transform;
	}

	transform *= local;
	this->transforms[nd] = transform;

	for (vector<AssimpNode*>::iterator it = nd->children.begin(); it != nd->children.end(); it++) {
		calcTransformNode((*it), transform, animTick);
	}
}


std::ostream &operator<< (std::ostream &out, const glm::mat4 &mat) {
    out << "{\n" 
        << mat[0].x << " " << mat[0].y << " " << mat[0].z << " " << mat[0].w << "\n"
        << mat[1].x << " " << mat[1].y << " " << mat[1].z << " " << mat[1].w << "\n"
        << mat[2].x << " " << mat[2].y << " " << mat[2].z << " " << mat[2].w << "\n"
        << mat[3].x << " " << mat[3].y << " " << mat[3].z << " " << mat[3].w << "\n"
        << "}";

    return out;
}


/**
* Calculate the bone transforms
**/
void AnimPlay::calcBoneTransforms()
{
	// set all bones to empty transform
	bone_transforms.clear();
	for (unsigned int i = 0; i < MAX_BONES; i++) {
		bone_transforms.push_back(glm::mat4(1.0f));
	}
	
	// For now we only operate on the first mesh
	AssimpMesh* mesh = this->model->meshes[0];
	
	for (unsigned int i = 0; i < mesh->bones.size(); i++) {
		AssimpBone *bn = mesh->bones[i];
		AssimpNode *nd = this->model->findNode(this->model->rootNode, bn->name);
		if (nd == NULL) continue;
		
		cout << "Bone " << i << ", node " << nd->name << "\n";
		
		glm::mat4 transform = bn->offset;
		
		AssimpNode* tempNode = nd;
		while (tempNode) {
			map<AssimpNode*, glm::mat4>::iterator it = this->transforms.find(tempNode);
			if (it != this->transforms.end()) {
				transform *= it->second;
			}
			
			tempNode = tempNode->parent;
		}
		
		this->bone_transforms[i] = transform;
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


