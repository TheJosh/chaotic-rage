// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <string>
#include <vector>
#include <SDL.h>
#include "util/ui_update.h"
#include "util/cmdline.h"
#include "util/clientconfig.h"
#include "util/util.h"
#include "mod/mod_manager.h"
#include "mod/mod.h"
#include "game_state.h"
#include "game_engine.h"
#include "game_manager.h"
#include "game_settings.h"
#include "render_opengl/menu.h"
#include "render_opengl/intro.h"
#include "i18n/gettext.h"
#include "audio/audio.h"

#ifdef RELEASE
	#include "http/client_stats.h"
#endif

#ifdef _WIN32
	#include <fstream>
	#include "util/stream_redirector.h"
#endif


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
	loadLang(GEng()->cconf->lang.c_str());

	#ifdef RELEASE
		// This has to be after the OpenGL init
		sendClientStats();
	#endif

	// Intro
	if (GEng()->render->is3D()) {
		Intro *i = new Intro(st);
		i->doit();
		ui = i;
	} else {
		ui = new UIUpdateNull();
	}

	// Load the mods
	if (!loadMods(st, ui)) {
		reportFatalError("Module loading failed");
	}
	delete(ui);
	ui = NULL;

	// Load rendering data used by all games and maps
	GEng()->render->loadCommonData();

	gm = new GameManager(st);

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
		if (c == NULL) {
			displayMessageBox("Campaign not found: " + GEng()->cmdline->campaign);
		} else {
			gm->startCampaign(c, "robot", GameSettings::firstPerson, 1);
		}

	// Arcade game
	} else if (GEng()->cmdline->map != "" && GEng()->cmdline->gametype != "" && GEng()->cmdline->unittype != "") {
		gm->loadModBits(NULL);
		GameSettings *gs = new GameSettings();

		MapReg* map = gm->getMapRegistry()->get(GEng()->cmdline->map);
		if (map == NULL) {
			displayMessageBox("Map not found: " + GEng()->cmdline->map);
		} else {
			gm->startGame(map, GEng()->cmdline->gametype, GEng()->cmdline->unittype, GameSettings::firstPerson, 1, GEng()->cmdline->host, gs);
		}
		delete(gs);

	// Network join
	} else if (GEng()->cmdline->join != "") {
		gm->loadModBits(NULL);
		gm->networkJoin(GEng()->cmdline->join, NULL);

	// Regular menu
	} else if (GEng()->render->is3D()) {
		Menu *m = new Menu(st, gm);
		m->doit(NULL);
		delete(m);

	} else {
		displayMessageBox("Non-interactive usage requires --campaign, --arcade or --join to be specified");
	}

	delete(st);
	delete(gm);
	delete(GEng()->render);
	delete(GEng()->audio);
	delete(GEng()->cmdline);
	delete(GEng()->mm);
	delete(GEng()->cconf);
	exit(0);
}
