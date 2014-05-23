// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <string>
#include <fstream>
#include <vector>
#include <SDL.h>
#include "rage.h"
#include "http/client_stats.h"
#include "util/ui_update.h"
#include "util/cmdline.h"
#include "util/clientconfig.h"
#include "util/stream_redirector.h"
#include "mod/mod_manager.h"
#include "mod/mod.h"
#include "map/map.h"
#include "game_state.h"
#include "game_engine.h"
#include "game_manager.h"
#include "game_settings.h"
#include "render_opengl/menu.h"
#include "render_opengl/intro.h"
#include "i18n/gettext.h"


using namespace std;


int main(int argc, char ** argv)
{
	GameState *st;
	UIUpdate *ui;
	GameManager* gm;


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

	new GameEngine();
	st = new GameState();

	// Parse command line args
	GEng()->cmdline = new CommandLineArgs(argc, argv);
	GEng()->cmdline->process();

	// Load render, audio, etc according to config
	GEng()->cconf = new ClientConfig();
	GEng()->cconf->initRender(st);
	GEng()->cconf->initAudio(st);
	GEng()->cconf->initPhysics(st);
	GEng()->cconf->initMods(st);

	// TODO: Add a config option for language
	loadLang("en");

	#ifdef RELEASE
		// This has to be after the OpenGL init
		sendClientStats();
	#endif

	// Intro
	if (GEng()->render->is3D()) {
		Intro *i = new Intro(st);
		i->load();
		i->doit();
		ui = i;
	} else {
		ui = new UIUpdateNull();
	}

	// Load the mods. Uses threads if possible
	if (!loadMods(st, ui)) {
		reportFatalError("Module loading failed");
	}

	gm = new GameManager(st);

	// For now, Android doesn't have a menu
	#if defined(__ANDROID__)
		gm->loadModBits(NULL);

		gm->startGame(gm->getMapRegistry()->get("therlor_valley"), "zombies", "robot", GameSettings::behindPlayer, 1, false, new GameSettings());
		exit(0);
	#endif

	// List of mods
	if (GEng()->cmdline->modlist) {
		cout << "Available mods:\n";
		vector<string>* modnames = GEng()->mm->getAvailMods();
		for (vector<string>::iterator it = modnames->begin(); it != modnames->end(); ++it) {
			cout << "    " << (*it) << "\n";
		}
		delete(modnames);

	// Campaign
	} else if (GEng()->cmdline->campaign != "") {
		gm->loadModBits(NULL);
		Campaign *c = GEng()->mm->getSupplOrBase()->getCampaign(GEng()->cmdline->campaign);
		gm->startCampaign(c, "robot", GameSettings::behindPlayer, 1);

	// Arcade game
	} else if (GEng()->cmdline->map != "" && GEng()->cmdline->gametype != "" && GEng()->cmdline->unittype != "") {
		gm->loadModBits(NULL);
		GameSettings *gs = new GameSettings();
		gm->startGame(gm->getMapRegistry()->get(GEng()->cmdline->map), GEng()->cmdline->gametype, GEng()->cmdline->unittype, GameSettings::behindPlayer, 1, GEng()->cmdline->host, gs);
		delete(gs);

	// Network join
	} else if (GEng()->cmdline->join != "") {
		gm->loadModBits(NULL);
		gm->networkJoin(GEng()->cmdline->join, NULL);

	// Regular menu
	} else if (GEng()->render->is3D()) {
		Menu *m = new Menu(st, gm);
		m->doit(NULL);

	} else {
		cout << "Non-interactive usage requires --campaign, --arcade or --join to be specified." << endl;
	}

	exit(0);
}

