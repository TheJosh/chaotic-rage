// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "mesh.h"
#include "../render_opengl/animplay.h"
#include "../render_opengl/assimpmodel.h"


/**
* Construct
**/
MapMesh::MapMesh(const glm::mat4 &xform, AssimpModel* model)
{
	this->xform = xform;
	this->model = model;
	this->play = new AnimPlay(this->model);
}

/**
* Destroy
**/
MapMesh::~MapMesh()
{
	delete this->play;
}
