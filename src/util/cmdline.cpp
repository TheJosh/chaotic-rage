// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <string>
#include <SDL.h>
#include <getopt.h>
#include "../rage.h"


using namespace std;



/**
* Parses the command line arguments using GetOpt
**/
void CommandLineArgs::process()
{
	#ifdef GETOPT
	
	
	static struct option long_options[] = {
		{"help",			0, 0, 'h'},
		{"mod",				1, 0, 'm'},
		{"mod-list",		0, 0, 'n'},
		{"campaign",		1, 0, 'c'},
		
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
					"Chaotic Rage\n\n"
					"Options:\n"
					" -h\t--help                 Show this help\n"
					" -m\t--mod NAME             Load a mod at startup\n"
					"   \t--mod-list             List all available mods, and exit\n"
					" -c\t--campaign NAME        Run the specified campaign, then exit\n"
					
					#ifdef DEBUG_OPTIONS
					"   \t--debug SECTION        Enable debug mode for a given section\n"
					"   \t--debug-list           Show a list of available debug sections\n"
					"   \t--debug-lineno         Show file and line no in debugging output\n"
					"   \t--debug-file           Save debug logs in a file instead of stdout\n"
					#endif
				;
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


