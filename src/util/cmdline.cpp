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
CommandLineArgs::CommandLineArgs(int argc, char ** argv)
{
	#ifdef GETOPT
	
	
	static struct option long_options[] = {
		{"help",			0, 0, 'h'},
		#ifdef DEBUG_OPTIONS
		{"debug",			1, 0, 'a'},
		{"debug-list",		0, 0, 'b'},
		{"debug-lineno",	0, 0, 'c'},
		{"debug-file",		1, 0, 'd'},
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
					#ifdef DEBUG_OPTIONS
					"   \t--debug SECTION        Enable debug mode for a given section\n"
					"   \t--debug-list           Show a list of available debug sections\n"
					"   \t--debug-lineno         Show file and line no in debugging output\n"
					"   \t--debug-file           Save debug logs in a file instead of stdout\n"
					#endif
				;
				exit(0);
				break;
			
			#ifdef DEBUG_OPTIONS
			case 'a':
				debug_enable(optarg);
				cout << "Enabling debug for '" << optarg << "'.\n";
				break;
				
			case 'b':
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
				
			case 'c':
				debug_lineno(true);
				cout << "Enabling file and line numbers in debug.\n";
				break;
				
			case 'd':
				debug_tofile(optarg);
				cout << "Saving debug data to file '" << optarg << "'.\n";
				break;
			#endif
			
			default: break;
		}
	}
	
	
	#endif
}


