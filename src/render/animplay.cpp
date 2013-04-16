// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include "../rage.h"

using namespace std;


AnimPlay::AnimPlay(AssimpModel * m)
{
	this->model = m;
}


/**
* Returns the model to render
**/
AssimpModel * AnimPlay::getModel()
{
	return this->model;
}
