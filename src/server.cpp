// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include "rage.h"


using namespace std;


void runGame(GameState * st, string map, string gametype);


int main (int argc, char ** argv)
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	
	seedRandom();
	
	GameState *st = new GameState();
	
	st->sconf = new ServerConfig();

	// Load render
	#ifdef NOGUI
		new RenderNull(st);
	#else
		new RenderDebug(st);
		st->render->setScreenSize(500, 500, false);
	#endif
	
	// Load other bits
	new AudioNull(st);
	new PhysicsBullet(st);
	new ModManager(st);
	
	// Load the mods, with threads if possible
	if (! threadedModLoader(st)) {
		exit(0);
	}
	
	// Run a game with the specified map and gametype
	runGame(st, st->sconf->map, st->sconf->gametype);
	
	exit(0);
}


/**
* Run a game with the specified map and gametype
**/
void runGame(GameState * st, string map, string gametype)
{
	new NetServer(st);
	
	st->physics->init();
	
	Map *m = new Map(st);
	m->load(map, st->render);
	st->curr_map = m;
	
	new GameLogic(st);
	GameType *gt = st->mm->getGameType(gametype);
	st->logic->execScript(gt->script);
	
	st->client = NULL;
	st->num_local = 0;
	
	st->physics->preGame();
	st->curr_map->preGame();

	st->server->listen(st->sconf->port);
	
	gameLoop(st, st->render);
	
	delete(st->server);
}

