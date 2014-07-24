// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <btBulletDynamicsCommon.h>

#include "mesh.h"

#include "../render_opengl/animplay.h"
#include "../render_opengl/assimpmodel.h"
#include "../util/convert.h"


/**
* Construct
**/
MapMesh::MapMesh(const glm::mat4 &xform, AssimpModel* model)
{
	this->xform = xform;
	this->model = model;
	this->play = new AnimPlay(this->model);
	this->trimesh = NULL;
	this->colshape = NULL;
	this->body = NULL;
}

/**
* Destroy
**/
MapMesh::~MapMesh()
{
	delete this->play;
	delete this->trimesh;
	delete this->colshape;
	delete this->body->getMotionState();
	delete this->body;
}


/**
* Create a rigid body for this mesh
**/
btRigidBody* MapMesh::createRigidBody()
{
	// Prepare the triangle mesh shape
	this->trimesh = new btTriangleMesh(false, false);
	this->fillTriangeMesh(trimesh, this->play, this->model, this->model->rootNode);
	this->colshape = new btBvhTriangleMeshShape(trimesh, true, true);

	// Prepare body
	btTransform xform;
	glmBullet(this->xform, xform);
	btDefaultMotionState* motionState = new btDefaultMotionState(xform);
	btRigidBody::btRigidBodyConstructionInfo meshRigidBodyCI(
		0.0f,
		motionState,
		this->colshape,
		btVector3(0.0f, 0.0f, 0.0f)
	);

	// Create body
	this->body = new btRigidBody(meshRigidBodyCI);
	this->body->setRestitution(0.f);
	this->body->setFriction(10.f);

	return this->body;
}


/**
* Fill a triangle mesh with triangles
*
* TODO: It would be better to use btTriangleIndexVertexArray or make AssimpModel implement btStridingMeshInterface
**/
void MapMesh::fillTriangeMesh(btTriangleMesh* trimesh, AnimPlay *ap, AssimpModel *am, AssimpNode *nd)
{
	glm::mat4 transform;
	glm::vec4 a, b, c;
	AssimpMesh* mesh;

	// Grab the transform for this node
	std::map<AssimpNode*, glm::mat4>::iterator local = ap->transforms.find(nd);
	assert(local != ap->transforms.end());
	transform = local->second;

	// Iterate the meshes and add triangles
	for (vector<unsigned int>::iterator it = nd->meshes.begin(); it != nd->meshes.end(); ++it) {
		mesh = am->meshes[(*it)];

		for (vector<AssimpFace>::iterator itt = mesh->faces->begin(); itt != mesh->faces->end(); ++itt) {
			a = transform * mesh->verticies->at((*itt).a);
			b = transform * mesh->verticies->at((*itt).b);
			c = transform * mesh->verticies->at((*itt).c);

			trimesh->addTriangle(btVector3(a.x, a.y, a.z), btVector3(b.x, b.y, b.z), btVector3(c.x, c.y, c.z));
		}
	}

	// Iterate children nodes
	for (vector<AssimpNode*>::iterator it = nd->children.begin(); it != nd->children.end(); ++it) {
		fillTriangeMesh(trimesh, ap, am, (*it));
	}
}

