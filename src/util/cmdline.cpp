// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "cmdline.h"
#include <cstring>
#include <getopt.h>
#include <iostream>
#include <sstream>
#include <string>
#include "../rage.h"
#include "debug.h"


using namespace std;


static void output_message(const char* msg) {
#ifdef WIN32
	MessageBox(HWND_DESKTOP, msg, "Chaotic Rage", MB_OK);
#else
	cout << msg;
#endif
}


/**
* Parses the command line arguments using GetOpt
**/
void CommandLineArgs::process()
{
	static const struct option long_options[] = {
		{"arcade",		    1, 0, 'a'},
		{"host",		    0, 0, 'n'},
		{"campaign",		1, 0, 'c'},
		{"join",		    1, 0, 'j'},
		{"throttle",        0, 0, 't'},

		{"mod",				1, 0, 'm'},
		{"mod-list",		0, 0, 'l'},

		{"render",			1, 0, 'r'},
		{"window",			2, 0, 'w'},
		{"fullscreen",		0, 0, 'f'},
		{"audio",			1, 0, 'u'},

		{"help",			0, 0, 'h'},
		{"version",			0, 0, 'v'},

		#ifdef DEBUG_OPTIONS
		{"debug",			1, 0, '1'},
		{"debug-list",		0, 0, '2'},
		{"debug-lineno",	0, 0, '3'},
		{"debug-file",		1, 0, '4'},
		{"no-mouse-grab",	0, 0, '5'},
		{"profile",         1, 0, '6'},
		{"loop-limit",      1, 0, '7'},
		#endif
		{NULL, 0, NULL, 0}
	};
	const char *short_options = "a:nc:j:tm:r:u:w::fhv";

	int c;
	int option_index = 0;
	while ((c = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) {
		switch (c) {
			case 'h':
				output_message(
					"Chaotic Rage " VERSION "\n"

					"\nOptions:\n"
					" -a\t--arcade MAP,GAME,UNIT  Play an arcade game, then exit\n"
					" -n\t--host                  Listen as a network host (requires --arcade)\n"
					" -c\t--campaign NAME         Run the specified campaign, then exit\n"
					" -j\t--join SERVER           Join a network game\n"
					" -t\t--throttle              Don't run the game loop at 100%, good for network servers\n"
					"\n"
					" -m\t--mod NAME              Load a mod at startup\n"
					"   \t--mod-list              List all available mods, and exit\n"
					"\n"
					" -r\t--render SYSTEM         Set the render system. Options: 'opengl', 'debug', 'null'\n"
					" -w\t--window[=W,H]          Display video in a window. Optional resolution\n"
					" -f\t--fullscreen            Display video in fullscreen\n"
					" -u\t--audio SYSTEM          Set the audio system. Options: 'sdl', 'null'\n"
					"\n"
					" -h\t--help                  Show this help\n"
					" -v\t--version               Show version, and exit\n"

					#ifdef DEBUG_OPTIONS
					"\nDebugging:\n"
					"   \t--debug SECTION         Enable debug mode for a given section\n"
					"   \t--debug-list            Show a list of available debug sections\n"
					"   \t--debug-lineno          Show file and line no in debugging output\n"
					"   \t--debug-file FILE       Save debug logs in a file instead of stdout\n"
					"   \t--no-mouse-grab         Disable mouse grab\n"
					"   \t--profile FILE          Save profile log\n"
					"   \t--loop-limit NUM        Limit iterations of the main game loop\n"
					#endif
				);
				exit(0);
				break;

			case 'v':
				cout << "Chaotic Rage " VERSION "\nhttp://www.chaoticrage.com/\n";
				exit(0);
				break;


			// Arcade, Host, Campaign, Join
			case 'a':
				{
					stringstream ss(optarg);
					getline(ss, this->map, ',');
					getline(ss, this->gametype, ',');
					getline(ss, this->unittype, ',');
				}
				break;

			case 'n':
				this->host = true;
				break;

			case 'c':
				this->campaign = optarg;
				break;

			case 'j':
				this->join = optarg;
				break;

			case 't':
				this->throttle = true;
				break;


			// Mod load, mod list
			case 'm':
				this->mod = optarg;
				break;

			case 'l':
				this->modlist = true;
				break;


			// Renderer
			case 'r':
				if (strcmp(optarg, "opengl") != 0 && strcmp(optarg, "debug") != 0 && strcmp(optarg, "null") != 0 && strcmp(optarg, "ascii") != 0) {
					cout << "Invalid render system." << endl;
					cout << "Options: 'opengl', 'debug', 'ascii', 'null'." << endl;
					exit(1);
				}
				this->render_class = optarg;
				break;

			case 'w':
				this->resolution[0] = 1;
				if (optarg != 0) {
					string tmp;
					stringstream ss(optarg);
					getline(ss, tmp, ',');
					this->resolution[1] = atoi(tmp.c_str());
					getline(ss, tmp, ',');
					this->resolution[2] = atoi(tmp.c_str());
				}
				break;

			case 'f':
				this->resolution[0] = 2;
				break;

			// Audio
			case 'u':
				if (strcmp(optarg, "sdl") != 0 && strcmp(optarg, "null") != 0) {
					cout << "Invalid audio system." << endl;
					cout << "Options: 'sdl', 'null'." << endl;
					exit(1);
				}
				this->audio_class = optarg;
				break;


			#ifdef DEBUG_OPTIONS
			case '1':
				debug_enable(optarg);
				cout << "Enabling debug for '" << optarg << "'.\n";
				break;

			case '2':
				cout <<
					"Chaotic Rage\n\n"
					"Debug Sections:\n"
					"  aud                Audio (Play, Stop)\n"
					"  coll               Collisions\n"
					"  data               Data-file loading\n"
					"  loop               Main game loop\n"
					"  lua                Lua function calls\n"
					"  mod                Mod loading\n"
					"  net                Network (also: net_pack, net_pkt, net_info)\n"
					"  phy                Physics (RigidBody adding, removing)\n"
					"  terrain            Terrain/Heightmap physics\n"
					"  unit               Unit movements\n"
					"  vid                Video\n"
					"  weap               Weapons\n"
					"  loadbones          Show bone structures during load\n"
				;
				exit(0);
				break;

			case '3':
				debug_lineno(true);
				cout << "Enabling file and line numbers in debug.\n";
				break;

			case '4':
				debug_tofile(optarg);
				cout << "Saving debug data to file '" << optarg << "'.\n";
				break;

			case '5':
				this->mouseGrab = false;
				break;

			case '6':
				profile_enable(optarg);
				cout << "Saving profile log to file '" << optarg << "'.\n";
				break;

			case '7':
				{
					int limit = atoi(optarg);
					mainloop_limit_enable(limit);
					cout << "Limiting iterations of the main loop to " << limit << ".\n";
				}
				break;
			#endif

			default: break;
		}
	}
}
