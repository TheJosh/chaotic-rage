// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <string>
#include <SDL.h>
#include "rage.h"


#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <guichan/opengl.hpp>
#include <guichan/opengl/openglsdlimageloader.hpp>


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
	new PhysicsBullet(st);
	new ModManager(st);
	
	st->render->setScreenSize(800, 600, false);
	
	
	// Load data
	Mod * mod = new Mod(st, "data/cr");
	if (! mod->load()) {
		reportFatalError("Unable to load data module 'cr'.");
	}
	st->mm->addMod(mod);
	
	// Init physics
	st->physics->init();
	
	// Load map
	Map *m = new Map(st);
	m->load("blank", st->render, NULL);
	st->curr_map = m;
	
	// Load gametype
	new GameLogic(st);
	GameType *gt = st->mm->getGameType("boredem");
	st->logic->execScript(gt->script);
	
	// Prep some env
	st->client = NULL;
	st->num_local = 1;
	st->local_players[0] = new PlayerState(st);
	st->local_players[0]->slot = 1;
	st->render->viewmode = 0;
	
	// Reset physics
	st->physics->preGame();
	
	// Create object type for anim model
	string tmp = argv[1];
	ObjectType *ot = new ObjectType();
	ot->name = tmp;
	ot->check_radius = 30;
	ot->health = 20000;
	ot->model = mod->getAssimpModel(tmp);
	if (ot->model == NULL) {
		reportFatalError("Animmodel not found");
	}
	mod->addObjectType(ot);
	
	// Create object in the world
	Object *o = new Object(ot, st, m->width/2, m->height/2, 0, 0);
	st->addObject(o);
	
	// Begin!
	gameLoop(st, st->render);
	
	exit(0);
}




