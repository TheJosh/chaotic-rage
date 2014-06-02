// Selected UTF-8 functions from SDL_TTF located at https://www.libsdl.org/projects/SDL_ttf/
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;


#include <SDL.h>


/*
  SDL_ttf:  A companion library to SDL for working with TrueType (tm) fonts
  Copyright (C) 1997-2013 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/



/* Gets a unicode value from a UTF-8 encoded string and advance the string */
#define UNKNOWN_UNICODE 0xFFFD
Uint32 UTF8_getch(const char **src, size_t *srclen)
{
    const Uint8 *p = *(const Uint8**)src;
    size_t left = 0;
    /*SDL_bool overlong = SDL_FALSE;*/
    SDL_bool underflow = SDL_FALSE;
    Uint32 ch = UNKNOWN_UNICODE;

    if (*srclen == 0) {
        return UNKNOWN_UNICODE;
    }
    if (p[0] >= 0xFC) {
        if ((p[0] & 0xFE) == 0xFC) {
            /*if (p[0] == 0xFC && (p[1] & 0xFC) == 0x80) {
                overlong = SDL_TRUE;
            }*/
            ch = (Uint32) (p[0] & 0x01);
            left = 5;
        }
    } else if (p[0] >= 0xF8) {
        if ((p[0] & 0xFC) == 0xF8) {
            /*if (p[0] == 0xF8 && (p[1] & 0xF8) == 0x80) {
                overlong = SDL_TRUE;
            }*/
            ch = (Uint32) (p[0] & 0x03);
            left = 4;
        }
    } else if (p[0] >= 0xF0) {
        if ((p[0] & 0xF8) == 0xF0) {
            /*if (p[0] == 0xF0 && (p[1] & 0xF0) == 0x80) {
                overlong = SDL_TRUE;
            }*/
            ch = (Uint32) (p[0] & 0x07);
            left = 3;
        }
    } else if (p[0] >= 0xE0) {
        if ((p[0] & 0xF0) == 0xE0) {
            /*if (p[0] == 0xE0 && (p[1] & 0xE0) == 0x80) {
                overlong = SDL_TRUE;
            }*/
            ch = (Uint32) (p[0] & 0x0F);
            left = 2;
        }
    } else if (p[0] >= 0xC0) {
        if ((p[0] & 0xE0) == 0xC0) {
            /*if ((p[0] & 0xDE) == 0xC0) {
                overlong = SDL_TRUE;
            }*/
            ch = (Uint32) (p[0] & 0x1F);
            left = 1;
        }
    } else {
        if ((p[0] & 0x80) == 0x00) {
            ch = (Uint32) p[0];
        }
    }
    ++*src;
    --*srclen;
    while (left > 0 && *srclen > 0) {
        ++p;
        if ((p[0] & 0xC0) != 0x80) {
            ch = UNKNOWN_UNICODE;
            break;
        }
        ch <<= 6;
        ch |= (p[0] & 0x3F);
        ++*src;
        --*srclen;
        --left;
    }
    if (left > 0) {
        underflow = SDL_TRUE;
    }
    /* Technically overlong sequences are invalid and should not be interpreted.
       However, it doesn't cause a security risk here and I don't see any harm in
       displaying them. The application is responsible for any other side effects
       of allowing overlong sequences (e.g. string compares failing, etc.)
       See bug 1931 for sample input that triggers this.
    */
    /*if (overlong) return UNKNOWN_UNICODE;*/
    if (underflow ||
        (ch >= 0xD800 && ch <= 0xDFFF) ||
        (ch == 0xFFFE || ch == 0xFFFF) || ch > 0x10FFFF) {
        ch = UNKNOWN_UNICODE;
    }
    return ch;
}

