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
	this->trimesh = new btTriangleIndexVertexArray();
	this->fillTriangeMesh(this->model->rootNode);
	this->colshape = new btBvhTriangleMeshShape(this->trimesh, true, true);

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
void MapMesh::fillTriangeMesh(AssimpNode *nd)
{
	AssimpMesh* asMesh;
	btIndexedMesh btMesh;

	// Iterate the meshes and add triangles
	for (vector<unsigned int>::iterator it = nd->meshes.begin(); it != nd->meshes.end(); ++it) {
		asMesh = this->model->meshes[(*it)];

		btMesh.m_numTriangles = asMesh->faces->size();
		btMesh.m_triangleIndexBase = (const unsigned char*) &asMesh->faces->at(0);
		btMesh.m_triangleIndexStride = sizeof(AssimpFace);
		btMesh.m_indexType = PHY_INTEGER;

		btMesh.m_numVertices = asMesh->verticies->size();
		btMesh.m_vertexBase = (const unsigned char*) &asMesh->verticies->at(0);
		btMesh.m_vertexStride = sizeof(glm::vec4);
		btMesh.m_vertexType = PHY_FLOAT;

		this->trimesh->addIndexedMesh(btMesh);
	}

	// Iterate children nodes
	for (vector<AssimpNode*>::iterator it = nd->children.begin(); it != nd->children.end(); ++it) {
		this->fillTriangeMesh(*it);
	}
}

