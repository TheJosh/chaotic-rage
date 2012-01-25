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
	
	#ifdef GETOPT
	st->cmdline = new CommandLineArgs(argc, argv);
	#endif
	
	new RenderOpenGL(st);
	new AudioSDLMixer(st);
	new HUD(st);
	new PhysicsBullet(st);
	new ModManager(st);

	
	st->render->setScreenSize(900, 900, false, 0);
	
	
	if (FEAT_INTRO) {
		Intro *i = new Intro(st);
		i->doit();
	}
	
	// Load data
	Mod * mod = new Mod(st, "data/cr");
	if (! mod->load()) {
		reportFatalError("Unable to load data module 'cr'.");
	}
	st->mm->addMod(mod);

	// Load user mods
	vector<string> * userfiles = getUserModFilenames();
	for (unsigned int i = 0; i < userfiles->size(); i++) {
		mod = new Mod(st, userfiles->at(i));
		if (! mod->load()) {
			reportFatalError("Unable to load data module '" + userfiles->at(i) + "'.");
		}
		st->mm->addMod(mod);
	}


	if (FEAT_MENU) {
		// Menu awesomeness
		Menu *m = new Menu(st);
		m->doit();
		
	} else {
		// Load map
		Map *m = new Map(st);
		m->load("blank", st->render);
		st->curr_map = m;
		
		// Load gametype
		new GameLogic(st);
		//GameType *gt = st->mm->getGameType("boredem");
		//st->logic->execScript(gt->script);
		
		new NetClient(st);
		st->client->bind("localhost", 17778);
		st->client->addmsgJoinReq();
		
		// Begin!
		gameLoop(st, st->render);
	}
	
	exit(0);
}




