// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <string>
#include <sstream>
#include <SDL.h>
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
		{"help",			0, 0, 'h'},
		{"version",			0, 0, 'v'},
		{"mod",				1, 0, 'm'},
		{"mod-list",		0, 0, 'n'},
		{"campaign",		1, 0, 'c'},
		{"arcade",		    1, 0, 'a'},
		{"join",		    1, 0, 'j'},
		
		#ifdef DEBUG_OPTIONS
		{"debug",			1, 0, 'w'},
		{"debug-list",		0, 0, 'x'},
		{"debug-lineno",	0, 0, 'y'},
		{"debug-file",		1, 0, 'z'},
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
					" -c\t--campaign NAME         Run the specified campaign, then exit\n"
					" -j\t--join SERVER           Join a network game\n"
					"\n"
					" -m\t--mod NAME              Load a mod at startup\n"
					"   \t--mod-list              List all available mods, and exit\n"
					" -h\t--help                  Show this help\n"
					" -v\t--version               Show version, and exit\n"
					
					#ifdef DEBUG_OPTIONS
					"\nDebugging:\n"
					"   \t--debug SECTION         Enable debug mode for a given section\n"
					"   \t--debug-list            Show a list of available debug sections\n"
					"   \t--debug-lineno          Show file and line no in debugging output\n"
					"   \t--debug-file            Save debug logs in a file instead of stdout\n"
					#endif
				;
				exit(0);
				break;
			
			case 'v':
				cout << "Chaotic Rage " VERSION "\nhttp://www.chaoticrage.com/\n";
				exit(0);
				break;
				
			case 'm':
				this->mod = optarg;
				break;
				
			case 'n':
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
				
			case 'c':
				this->campaign = optarg;
				break;
				
			case 'a':
				{
					std::stringstream ss(optarg);
					std::getline(ss, this->map, ',');
					std::getline(ss, this->gametype, ',');
					std::getline(ss, this->unittype, ',');
				}
				break;
				
			case 'j':
				this->join = optarg;
				break;
				
				
			#ifdef DEBUG_OPTIONS
			case 'w':
				debug_enable(optarg);
				cout << "Enabling debug for '" << optarg << "'.\n";
				break;
				
			case 'x':
				cout <<
					"Chaotic Rage\n\n"
					"Debug Sections:\n"
					"  aud                Audio (Play, Stop)\n"
					"  coll               Collisions\n"
					"  data               Data-file loading\n"
					"  lua                Lua function calls\n"
					"  phy                Physics (RigidBody adding, removing)\n"
					"  unit               Unit movements\n"
					"  vid                Video\n"
					"  loop               Main game loop\n"
					"  net                Network (also: net_pack)\n"
					"  weap               Weapons\n"
					"  terrain            Terrain/Heightmap physics\n"
				;
				exit(0);
				break;
				
			case 'y':
				debug_lineno(true);
				cout << "Enabling file and line numbers in debug.\n";
				break;
				
			case 'z':
				debug_tofile(optarg);
				cout << "Saving debug data to file '" << optarg << "'.\n";
				break;
			#endif
			
			default: break;
		}
	}
	
	
	#endif
}


