// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

using namespace std;

#include <glm/glm.hpp>


/**
* Configuration for the Assimp model loader
**/
class AssimpLoadConfig
{
	public:
		// Flatten and simplify geometry?
		bool flattenGeometry = false;

		// Adjust model so the centre-point is correct?
		bool recenter = true;

		// Load mesh data into an array? Needed if wanting to
		// create a physics mesh from the model
		bool meshdata = false;

		// Amount to scale the model, 1.0 is no scaling
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
};
