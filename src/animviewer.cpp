// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <string>
#include <SDL.h>
#include "rage.h"


using namespace std;



int main (int argc, char * argv[])
{
	if (argc != 2) {
		cout << "Usage:\n" << argv[0] << " [model]\n";
		exit(1);
	}
	
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	
	seedRandom();
	
	GameState *st = new GameState();
	
	new RenderOpenGL(st);
	new AudioSDLMixer(st);
	new HUD(st);
	new PhysicsBullet(st);
	
	st->render->setScreenSize(900, 900, false, 0);
	
	Mod * mod = new Mod(st, "data/cr");
	
	// Load data
	if (! mod->load()) {
		reportFatalError("Unable to load data module 'cr'.");
	}
	
	
	st->physics->preGame();
	
	// Load map
	Map *m = new Map(st);
	m->load("blank", st->render);
	st->curr_map = m;
	
	new GameLogic(st);
	GameType *gt = st->mm->getGameType("boredem");
	st->logic->execScript(gt->script);
	
	st->client = NULL;
	st->num_local = 1;
	st->local_players[0] = NULL;
	
	((RenderOpenGL*)st->render)->viewmode = 1;
	
	
	// Create object type for anim model
	string tmp = argv[1];
	ObjectType *ot = new ObjectType();
	ot->name = tmp;
	ot->check_radius = 30;
	ot->health = 20000;
	
	ot->model = mod->getAnimModel(tmp);
	if (ot->model == NULL) {
		reportFatalError("Animmodel not found");
	}
	
	mod->addObjectType(ot);
	
	
	// Create object in the world
	Object *o = new Object(ot, st, m->width/2, m->height/2, 0);
	st->addObject(o);
	
	
	// Begin!
	gameLoop(st, st->render);
	
	
	exit(0);
}




