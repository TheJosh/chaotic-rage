// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <string>
#include <sstream>
#include <iostream>
#include <getopt.h>
#include "../rage.h"
#include "../gamestate.h"
#include "../mod/mod_manager.h"
#include "cmdline.h"


using namespace std;



/**
* Parses the command line arguments using GetOpt
**/
void CommandLineArgs::process()
{
	#ifdef GETOPT
	
	
	static struct option long_options[] = {
		{"arcade",		    1, 0, 'a'},
		{"host",		    0, 0, 'n'},
		{"campaign",		1, 0, 'c'},
		{"join",		    1, 0, 'j'},
		
		{"mod",				1, 0, 'm'},
		{"mod-list",		0, 0, 'l'},
		
		{"render",			1, 0, 'r'},
		{"audio",			1, 0, 'u'},
		
		{"help",			0, 0, 'h'},
		{"version",			0, 0, 'v'},
		
		#ifdef DEBUG_OPTIONS
		{"debug",			1, 0, '1'},
		{"debug-list",		0, 0, '2'},
		{"debug-lineno",	0, 0, '3'},
		{"debug-file",		1, 0, '4'},
		{"no-mouse-grab",	0, 0, '5'},
		#endif
		{NULL, 0, NULL, 0}
	};
	
	int c;
	int option_index = 0;
	while ((c = getopt_long(argc, argv, "h", long_options, &option_index)) != -1) {
		switch (c) {
			case 'h':
				cout <<
					"Chaotic Rage " VERSION "\n"
					
					"\nOptions:\n"
					" -a\t--arcade MAP,GAME,UNIT  Play an arcade game, then exit\n"
					" -n\t--host                  Listen as a network host (requires --arcade)\n"
					" -c\t--campaign NAME         Run the specified campaign, then exit\n"
					" -j\t--join SERVER           Join a network game\n"
					"\n"
					" -m\t--mod NAME              Load a mod at startup\n"
					"   \t--mod-list              List all available mods, and exit\n"
					"\n"
					" -r\t--render SYSTEM         Set the render system. Options: 'opengl', 'debug', 'null'\n"
					" -u\t--audio SYSTEM          Set the audio system. Options: 'sdl', 'null'\n"
					"\n"
					" -h\t--help                  Show this help\n"
					" -v\t--version               Show version, and exit\n"
					
					#ifdef DEBUG_OPTIONS
					"\nDebugging:\n"
					"   \t--debug SECTION         Enable debug mode for a given section\n"
					"   \t--debug-list            Show a list of available debug sections\n"
					"   \t--debug-lineno          Show file and line no in debugging output\n"
					"   \t--debug-file            Save debug logs in a file instead of stdout\n"
					"   \t--no-mouse-grab         Disable mouse grab\n"
					#endif
				;
				exit(0);
				break;
			
			case 'v':
				cout << "Chaotic Rage " VERSION "\nhttp://www.chaoticrage.com/\n";
				exit(0);
				break;
				
			
			// Arcade, Host, Campaign, Join
			case 'a':
				{
					std::stringstream ss(optarg);
					std::getline(ss, this->map, ',');
					std::getline(ss, this->gametype, ',');
					std::getline(ss, this->unittype, ',');
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
				
				
			// Mod load, mod list
			case 'm':
				this->mod = optarg;
				break;
				
			case 'l':
				{
					cout << "Available mods:\n";
					vector<string>* modnames = st->mm->getAvailMods();
					for (vector<string>::iterator it = modnames->begin(); it != modnames->end(); it++) {
						cout << "    " << (*it) << "\n";
					}
					delete(modnames);
					exit(0);
				}
				break;
				
				
			// Renderer
			case 'r':
				if (strcmp(optarg, "opengl") != 0 && strcmp(optarg, "debug") != 0 && strcmp(optarg, "null") != 0) {
					cout << "Invalid render system." << endl;
					cout << "Options: 'opengl', 'debug', 'null'." << endl;
					exit(1);
				}
				this->render = optarg;
				break;
				
			// Audio
			case 'u':
				if (strcmp(optarg, "sdl") != 0 && strcmp(optarg, "null") != 0) {
					cout << "Invalid audio system." << endl;
					cout << "Options: 'sdl', 'null'." << endl;
					exit(1);
				}
				this->audio = optarg;
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
			#endif
			
			default: break;
		}
	}
	
	
	#endif
}


