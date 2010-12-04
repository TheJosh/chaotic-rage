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
	
	new RenderOpenGL(st);
	new AudioSDLMixer(st);
	
	st->render->setScreenSize(900, 900, false);
	
	Mod * mod = new Mod(st, "data/cr/");
	
	if (! mod->load()) {
		cerr << "Unable to load datafile.\n";
		exit(1);
	}
	
	
	Map *m = new Map(st);
	m->load("arena", st->render);
	st->map = m;
	
	new GameLogic(st);
	
	gameLoop(st, st->render);
	
	exit(0);
}




