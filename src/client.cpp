// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <string>
#include <fstream>
#include <SDL.h>
#include "rage.h"
#include "http/client_stats.h"
#include "util/ui_update.h"
#include "util/cmdline.h"
#include "util/clientconfig.h"
#include "util/stream_redirector.h"
#include "mod/mod_manager.h"
#include "mod/mod.h"
#include "map.h"
#include "gamestate.h"
#include "menu.h"
#include "intro.h"



using namespace std;


int main (int argc, char ** argv)
{
	GameState *st;
	UIUpdate *ui;
	Intro *i;
	Menu *m;
	
	// Redirect stdout and stderr to files in the user directory
	#ifdef _WIN32
		string datadir = getUserDataDir();
		string nameOut = datadir + "stdout.txt"; 
		string nameErr = datadir + "stderr.txt"; 
		ofstream fileOut(nameOut.c_str());
		ofstream fileErr(nameErr.c_str());
		StreamRedirector redirOut(cout, fileOut.rdbuf());
		StreamRedirector redirErr(cerr, fileErr.rdbuf());
	#endif
	
	
	chdirToDataDir();
	SDL_Init(0);
	seedRandom();
	
	st = new GameState();
	st->cmdline = new CommandLineArgs(st, argc, argv);
	st->cconf = new ClientConfig();
	
	#ifdef GETOPT
		st->cmdline->process();
	#endif
	
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
	if (st->render->is3D()) {
		ui = i = new Intro(st);
		i->load();
		i->doit();
	} else {
		ui = new UIUpdateNull();
	}
	
	// Load the mods. Uses threads if possible
	if (! loadMods(st, ui)) {
		exit(0);
	}

	m = new Menu(st);
	
	// For now, Android doesn't have a menu
	#if defined(__ANDROID__)
		m->loadModBits();
		m->startGame(m->mapreg->get("therlor_valley"), "free_for_all", "robot", 0, 1, false);
		exit(0);
	#endif

	// Campaign
	if (st->cmdline->campaign != "") {
		m->loadModBits();
		Campaign *c = st->mm->getSupplOrBase()->getCampaign(st->cmdline->campaign);
		m->startCampaign(c, "robot", 0, 1);
		
	// Arcade game
	} else if (st->cmdline->map != "" && st->cmdline->gametype != "" && st->cmdline->unittype != "") {
		m->loadModBits();
		m->startGame(m->mapreg->get(st->cmdline->map), st->cmdline->gametype, st->cmdline->unittype, 0, 1, st->cmdline->host);
		
	// Network join
	} else if (st->cmdline->join != "") {
		m->loadModBits();
		m->networkJoin(st->cmdline->join);
		
	// Regular menu
	} else if (st->render->is3D()) {
		m->doit(i);
		
	} else {
		cout << "Non-interactive usage requires --campaign, --arcade or --join to be specified." << endl;
	}
	
	exit(0);
}

