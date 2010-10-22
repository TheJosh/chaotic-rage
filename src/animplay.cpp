// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


AnimPlay::AnimPlay(AnimModel * m)
{
	this->model = m;
	this->start_frame = -1;
}


/**
* Returns the model to render
**/
AnimModel * AnimPlay::getModel()
{
	return this->model;
}

/**
* Returns the currently playing frame number
**/
int AnimPlay::getFrame()
{
	// Only start the animation once it's actually rendered
	// Allows them to be set up in constructors, etc, which may be several hundred ms before the
	// game starts
	if (this->start_frame == -1) this->start_frame = getGameState()->anim_frame;
	
	
	int framenum = getGameState()->anim_frame - this->start_frame;
	
	if (this->model->next == NULL) {
		framenum = framenum % this->model->num_frames;
		
	} else if (framenum >= this->model->num_frames) {
		framenum = this->model->num_frames - 1;
	}
	
	return framenum;
}

/**
* Return TRUE if the animation is playing, FALSE if it has stopped
**/
bool AnimPlay::isPlaying()
{
	if (this->model->next == NULL) return true;
	
	int framenum = getGameState()->anim_frame - this->start_frame;
	if (framenum > this->model->num_frames) {
		return false;
	}
	
	return true;
}
