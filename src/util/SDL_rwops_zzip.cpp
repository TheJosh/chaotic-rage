/*
 *	  Copyright (c) 2001 Guido Draheim <guidod@gmx.de>
 *	  Use freely under the restrictions of the ZLIB License
 *
 *	  (this example uses errno which might not be multithreaded everywhere)
 */

#include "SDL_rwops_zzip.h"
#include <zzip/zzip.h>
#include <string.h> /* strchr */

#ifndef O_BINARY
#define O_BINARY 0
#endif


// Allowed 'zip' extentions
static zzip_strings_t mod_zzip_ext[] = { ".crk", ".CRK", 0 };


/* MSVC can not take a casted variable as an lvalue ! */
#define SDL_RWOPS_ZZIP_DATA(_context) \
			 ((_context)->hidden.unknown.data1)
#define SDL_RWOPS_ZZIP_FILE(_context)  (ZZIP_FILE*) \
			 ((_context)->hidden.unknown.data1)


static int sdl_rwops_zzip_seek(SDL_RWops *context, int offset, int whence)
{
	return zzip_seek(SDL_RWOPS_ZZIP_FILE(context), offset, whence);
}

static int sdl_rwops_zzip_read(SDL_RWops *context, void *ptr, int size, int maxnum)
{
	return zzip_read(SDL_RWOPS_ZZIP_FILE(context), ptr, size*maxnum) / size;
}

static int sdl_rwops_zzip_write(SDL_RWops *context, const void *ptr, int size, int num)
{
	return 0; /* ignored */
}

static int sdl_rwops_zzip_close(SDL_RWops *context)
{
	if (! context) return 0; /* may be SDL_RWclose is called by atexit */

	zzip_close (SDL_RWOPS_ZZIP_FILE(context));
	SDL_FreeRW (context);
	return 0;
}

SDL_RWops *SDL_RWFromZZIP(const char* file, const char* mode)
{
	register SDL_RWops* rwops;
	register ZZIP_FILE* zzip_file;

	zzip_file = zzip_open_ext_io (file, O_RDONLY|O_BINARY, 0, mod_zzip_ext, 0);
	if (! zzip_file) return 0;

	rwops = SDL_AllocRW ();
	if (! rwops) { errno=ENOMEM; zzip_close (zzip_file); return 0; }

	SDL_RWOPS_ZZIP_DATA(rwops) = zzip_file;
	rwops->read = sdl_rwops_zzip_read;
	rwops->write = sdl_rwops_zzip_write;
	rwops->seek = sdl_rwops_zzip_seek;
	rwops->close = sdl_rwops_zzip_close;

	return rwops;
}
