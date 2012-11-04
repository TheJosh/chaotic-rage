// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include "rage.h"


using namespace std;



int main (int argc, char ** argv) {
	
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	
	seedRandom();
	
	GameState *st = new GameState();
	st->curr_slot = 0;
	
	new RenderDebug(st);
	new AudioNull(st);
	new PhysicsBullet(st);
	new ModManager(st);
	new NetServer(st);
	
	st->render->setScreenSize(500, 500, false);
	
	
	// Load data
	Mod * mod = new Mod(st, "data/cr");
	if (! mod->load()) {
		reportFatalError("Unable to load data module 'cr'.");
	}
	st->mm->addMod(mod);
	
	
	st->physics->init();
	
	Map *m = new Map(st);
	m->load("tanktest", st->render);
	st->curr_map = m;
	
	new GameLogic(st);
	GameType *gt = st->mm->getGameType("boredem");
	st->logic->execScript(gt->script);
	
	st->client = NULL;
	st->num_local = 0;
	
	st->physics->preGame();
	
	st->server->listen(17778);
	
	gameLoop(st, st->render);
	
	exit(0);
}




