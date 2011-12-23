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

