#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <iostream>

int main(int argc, char** argv)
{
	Assimp::Importer importer;
	const char * sphere = "s 1 1 1 5";

	const struct aiScene* sc = importer.ReadFileFromMemory(sphere, strlen(sphere), 0, "sphere.nff");
	
	std::cout << "Meshes: " << sc->mNumMeshes << std::endl;
}

