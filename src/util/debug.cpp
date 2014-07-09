// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <btBulletDynamicsCommon.h>
#include <cstdarg>
#include <algorithm>
#include "../rage.h"

using namespace std;


static list<string> enabled_sects;
static bool lineno;
static FILE* out = stdout;
static FILE* profile = NULL;
static bool mainloop_limit_enabled = false;
static int mainloop_limit = 0;


void write_debug(const char * sect, const char * file, int line, const char * fmt, ...)
{
	list<string>::iterator result = find(enabled_sects.begin(), enabled_sects.end(), sect);
	if (result == enabled_sects.end()) return;

	char msg[1024 * 16];
	va_list argptr;

	va_start(argptr, fmt);
	vsprintf(msg, fmt, argptr);
	va_end(argptr);

	if (lineno) {
		fprintf(out, "[%4s] %s:%i   %s\n", sect, file, line, msg);
	} else {
		fprintf(out, "[%4s] %s\n", sect, msg);
	}

	fflush(out);
}

void debug_enable(const char * sect)
{
	enabled_sects.push_back(sect);
}

bool debug_enabled(const char * sect)
{
	return (std::find(enabled_sects.begin(), enabled_sects.end(), sect) != enabled_sects.end());
}

void debug_lineno(bool enabled)
{
	lineno = enabled;
}

void debug_tofile(const char * filename)
{
	out = fopen(filename, "w");
	atexit(debug_save);
}

void debug_save()
{
	fclose(out);
}

void profile_enable(const char * filename)
{
	profile = fopen(filename, "w");
	atexit(profile_save);
	fprintf(profile, "Event\tStartTime\tEndTime\tElapsed\n");
}

void profile_write(const char * sect, int start, int end)
{
	if (profile != NULL) {
		fprintf(profile, "%s\t%i\t%i\t%i\n", sect, start, end, end-start);
	}
}

void profile_save()
{
	fclose(profile);
}


void mainloop_limit_enable(int limit)
{
	mainloop_limit_enabled = true;
	mainloop_limit = limit;
}

// return TRUE to break the loop
bool mainloop_limit_iter()
{
	if (mainloop_limit_enabled) {
		--mainloop_limit;
		if (mainloop_limit == 0) {
			cout << "Main loop iteration limit reached; exiting.\n"; 
			return true;
		}
	}

	return false;
}

