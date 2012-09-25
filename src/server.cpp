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
	new NetServer(st);
	
	
	st->render->setScreenSize(900, 900, false, 0);
	
	Mod * mod = new Mod(st, "data/cr");
	
	if (! mod->load()) {
		cerr << "Unable to load datafile.\n";
		exit(1);
	}
	
	
	Map *m = new Map(st);
	m->load("blank", st->render);
	st->curr_map = m;
	
	new GameLogic(st);
	GameType *gt = st->mm->getGameType("boredem");
	st->logic->execScript(gt->script);
	
	st->server->listen(17778);
	
	gameLoop(st, st->render);
	
	exit(0);
}




