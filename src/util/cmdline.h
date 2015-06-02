// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <string>

using namespace std;



class CommandLineArgs
{
	private:
		int argc;
		char ** argv;

	public:
		string mod;
		bool modlist;
		string main_mod_path;
		string campaign;
		string map;
		string gametype;
		string unittype;
		string join;
		string render_class;
		string audio_class;
		bool host;
		bool mouseGrab;
		bool throttle;
		int resolution[3];

	public:
		CommandLineArgs(int argc, char ** argv)
			: argc(argc), argv(argv), modlist(false), host(false), mouseGrab(true), throttle(false)
		{
			this->resolution[0] = 0;		// 0 = config, 1 = windowed, 2 = fullscreen
			this->resolution[1] = 0;		// x
			this->resolution[2] = 0;		// y
			this->main_mod_path = "data/cr";
		}

		void process();
};
