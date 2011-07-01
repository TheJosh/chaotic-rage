// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <string>
#include <SDL.h>
#include "rage.h"


using namespace std;



int main (int argc, char ** argv)
{
	cout << ".";
	cerr << ".";

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	
	seedRandom();
	
	GameState *st = new GameState();
	
	new RenderOpenGL(st);
	new AudioSDLMixer(st);
	new NetClient(st);
	
	// todo: change to match others...
	st->hud = new HUD();
	st->hud->st = st;
	
	
	st->render->setScreenSize(900, 900, false);
	
	Mod * mod = new Mod(st, "data/cr");
	
	if (FEAT_INTRO) {
		Intro *i = new Intro(st);
		i->doit();
	}
	
	// Load data
	if (! mod->load()) {
		reportFatalError("Unable to load data module 'cr'.");
	}
	
	if (FEAT_MENU) {
		// Menu awesomeness
		Menu *m = new Menu(st);
		m->doit();
		
	} else {
		// Load map
		Map *m = new Map(st);
		m->load("test", st->render);
		st->curr_map = m;
		
		// Load gametype
		new GameLogic(st);
		//GameType *gt = st->getDefaultMod()->getGameType("boredem");
		//st->logic->execScript(gt->script);
		
		st->client->bind("localhost", 17778);
		st->client->addmsgInfoReq();
		
		// Begin!
		gameLoop(st, st->render);
	}
	
	exit(0);
}




