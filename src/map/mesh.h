// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <glm/glm.hpp>
#include <string>
#include "../rage.h"

using namespace std;


class AssimpModel;
class AssimpNode;
class AnimPlay;
class btRigidBody;


class MapMesh {
	friend class RenderOpenGL;
	friend class Map;

	private:
		// Positon and rotation
		glm::mat4 xform;

		// Render
		AssimpModel* model;
		AnimPlay* play;

		// Physics
		btRigidBody* body;
		btCollisionShape* colshape;
		btTriangleIndexVertexArray* trimesh;

	public:
		MapMesh(const glm::mat4 &xform, AssimpModel* model);
		~MapMesh();

		/**
		* Create a rigid body for this mesh
		**/
		btRigidBody* createRigidBody();

	private:
		void fillTriangeMesh(AssimpNode *nd);
};
