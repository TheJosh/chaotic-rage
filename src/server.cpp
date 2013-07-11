// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include "rage.h"
#include "gamestate.h"
#include "menu.h"
#include "intro.h"
#include "render/render_debug.h"
#include "audio/audio_null.h"

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
	if (! loadMods(st, NULL)) {
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
	m->load(map, st->render, NULL);
	st->map = m;
	
	new GameLogic(st);
	GameType *gt = st->mm->getGameType(gametype);
	st->logic->execScript(gt->script);
	
	st->client = NULL;
	st->num_local = 0;
	
	gameLoop(st, st->render);
	
	delete(st->server);
}

