// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <string>
#include <SDL.h>
#include <getopt.h>
#include "rage.h"


using namespace std;



/**
* Parses the command line arguments using GetOpt
**/
CommandLineArgs::CommandLineArgs(int argc, char ** argv)
{
	this->debug = false;
	
	
	
	static struct option long_options[] = {
		{"help",	0, 0, 'h'},
		{"debug",	0, 0, 'd'},
		{NULL, 0, NULL, 0}
	};
	
	int c;
	int option_index = 0;
	while ((c = getopt_long(argc, argv, "hd", long_options, &option_index)) != -1) {
		switch (c) {
			case 'h':
				cout <<
					"Chaotic Rage\n\n"
					"Options:\n"
					" -h --help\t\tShow this help\n"
					" -d --debug\t\tEnable debug mode\n";
					exit(0);
				break;
			
			case 'd':
				this->debug = true;
				break;
			
			default: break;
		}
	}
	
	if (this->debug) {
		cout << "Debug mode ON\n";
	}
}


