// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <string>
#include <SDL.h>
#include "rage.h"
#include "http/client_stats.h"
#include "util/windowicon.h"
#include "util/ui_update.h"
#include "gamestate.h"
#include "menu.h"
#include "intro.h"

using namespace std;


int main (int argc, char ** argv)
{
	cout << ".";
	cerr << ".";
	
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	SDL_EnableUNICODE(1);

	seedRandom();
	
	// Load icon
	SDL_RWops *rw = SDL_RWFromConstMem(windowicon_bmp, sizeof(windowicon_bmp));
	SDL_Surface *icon = SDL_LoadBMP_RW(rw, 1);
	SDL_SetColorKey(icon, SDL_SRCCOLORKEY, SDL_MapRGBA(icon->format, 255, 0, 255, 0));
	SDL_WM_SetIcon(icon, NULL);
	SDL_FreeSurface(icon);
	
	GameState *st = new GameState();
	
	st->cmdline = new CommandLineArgs(st, argc, argv);
	
	#ifdef GETOPT
	st->cmdline->process();
	#endif
	
	st->cconf = new ClientConfig();

	// Load render, audio, etc according to config
	st->cconf->initRender(st);
	st->cconf->initAudio(st);
	st->cconf->initPhysics(st);
	st->cconf->initMods(st);
	
	#ifdef RELEASE
		// This has to be after the OpenGL init
		sendClientStats();
	#endif
	
	// Intro
	Intro *i = new Intro(st);
	i->load();
	i->doit();

	// Load the mods, with threads if possible
	if (! loadMods(st, i)) {
		exit(0);
	}

	Menu *m = new Menu(st);
	
	// If a campaign or arcade game has been specified on the cmd line, run it
	if (st->cmdline != NULL) {
		if (st->cmdline->campaign != "") {
			m->loadModBits();
			Campaign *c = st->mm->getSupplOrBase()->getCampaign(st->cmdline->campaign);
			m->startCampaign(c, "robot", 0, 1);
			exit(0);
			
		} else if (st->cmdline->map != "" && st->cmdline->gametype != "" && st->cmdline->unittype != "") {
			m->loadModBits();
			m->startGame(m->mapreg->get(st->cmdline->map), st->cmdline->gametype, st->cmdline->unittype, 0, 1);
			exit(0);
		}
	}
	
	m->doit(i);
	
	exit(0);
}


