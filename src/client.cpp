// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <string>
#include <SDL.h>
#include "rage.h"
#include "http/client_stats.h"
#include "util/windowicon.h"

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
	
	//displayMessageBox("Basic init done, let's init the subsystems.");

	GameState *st = new GameState();
	
	#ifdef GETOPT
	st->cmdline = new CommandLineArgs(argc, argv);
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
	
	//displayMessageBox("Init of all subsystems done, loading intro");

	// Intro
	Intro *i = new Intro(st);
	i->load();
	i->doit();

	//displayMessageBox("Intro finished, loading game data");

	// Load the mods, with threads if possible
	if (! threadedModLoader(st)) {
		exit(0);
	}

	//displayMessageBox("That's done, let's show the menu!");

	Menu *m = new Menu(st);
	
	// If you have a lot of debugging work to do, uncomment this
	// and change your settings as required.
	//   map, gametype, character, viewmode, num players
	//m->startGame("tanktest", "boredem", "robot", 0, 1);
	
	// Run the menu
	m->doit();
	
	exit(0);
}


