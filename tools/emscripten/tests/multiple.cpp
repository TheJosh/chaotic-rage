#include <iostream>
#include <stdlib.h>

#include <SDL/SDL.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}

#include <ft2build.h>
#include FT_FREETYPE_H

#include "btBulletDynamicsCommon.h"


int main (int argc, char ** argv)
{
	std::cout << "Emscripten okay" << std::endl;

	// SDL
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Surface *screen = SDL_SetVideoMode(600, 450, 32, SDL_SWSURFACE);
	if (SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j < 256; j++) {
			*((Uint32*)screen->pixels + i * 256 + j) = SDL_MapRGBA(screen->format, i, j, 255-i, (i+j) % 255);
		}
	}
	if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
	SDL_Flip(screen);
	std::cout << "SDL okay" << std::endl;

	// Assimp
	Assimp::Importer importer;
	const char * sphere = "s 1 1 1 5";
	const struct aiScene* sc = importer.ReadFileFromMemory(sphere, strlen(sphere), 0, "sphere.nff");
	std::cout << "Assimp okay" << std::endl;

	// Lua
	static lua_State *L = luaL_newstate();
	std::cout << "Lua okay" << std::endl;
	lua_close(L);

	// Freetype
	int error;
	FT_Library ft;
	error = FT_Init_FreeType(&ft);
	if (!error) {
		std::cerr << "Freetype okay" << std::endl;
		FT_Done_FreeType(ft);
	}

	// Bullet
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new	btCollisionDispatcher(collisionConfiguration);
	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,overlappingPairCache,solver,collisionConfiguration);
	delete dynamicsWorld;
	delete solver;
	delete overlappingPairCache;
	delete dispatcher;
	delete collisionConfiguration;
	std::cerr << "Bullet okay" << std::endl;

	SDL_Quit();
	exit(0);
}
