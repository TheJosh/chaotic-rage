// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include "../rage.h"
#include "../game_state.h"
#include "../map/map.h"
#include "../entity/entity.h"
#include "../entity/unit.h"
#include "render_ascii.h"
#include "sprite.h"
#include <SDL.h>
#include <SDL_image.h>

#ifdef __linux__
	#include <sys/ioctl.h>
	#include <stdio.h>
	#include <unistd.h>
#endif

using namespace std;



/**
* Basic setup. We create the buffer in the setScreenSize call
**/
RenderAscii::RenderAscii(GameState * st) : Render(st)
{
	this->last_render = 0;
	this->width = 0;
	this->height = 0;
	this->buffer = NULL;
	this->color = NULL;
}


/**
* Free the backbuffer
**/
RenderAscii::~RenderAscii()
{
	free(this->buffer);
	free(this->color);
}


/**
* Sets the screen size, creates the backbuffer, and does a terminal clear and reset
**/
void RenderAscii::setScreenSize(int width, int height, bool fullscreen)
{
	free(this->buffer);
	free(this->color);

	#ifdef __gnu_linux__
		struct winsize w;
		ioctl(0, TIOCGWINSZ, &w);
		w.ws_row -= 2;

		if (w.ws_row <= 2 || w.ws_col <= 2) {
			printf("Terminal too small");
			exit(1);
		}

		if ((w.ws_col/2) < w.ws_row) {
			this->width = w.ws_col;
			this->height = w.ws_col/2;
		} else {
			this->width = w.ws_row*2;
			this->height = w.ws_row;
		}
	#else
		this->width = 60;
		this->height = 30;
	#endif

	this->buffer = (char*)malloc(this->width * this->height);
	this->color = (Uint8*)malloc(this->width * this->height);

	printf("\033[0m\033[2J");
}


/**
* Internal sprite loading from a SDL_RWops
**/
SpritePtr RenderAscii::int_loadSprite(SDL_RWops *rw, string filename)
{
	SDL_Surface * surf;

	surf = IMG_Load_RW(rw, 0);
	if (surf == NULL) {
		fprintf(stderr, "Couldn't load sprite '%s'\n", filename.c_str());
		load_err = true;
		return NULL;
	}

	// Create the sprite object
	SpritePtr sprite = new Sprite();
	sprite->w = surf->w;
	sprite->h = surf->h;
	sprite->orig = surf;

	return sprite;
}


/**
* Renders a sprite.
* Should only be used if the the caller was called by this classes 'Render' function.
**/
void RenderAscii::renderSprite(SpritePtr sprite, int x, int y)
{
}


/**
* Renders a sprite.
* Should only be used if the the caller was called by this classes 'Render' function.
**/
void RenderAscii::renderSprite(SpritePtr sprite, int x, int y, int w, int h)
{
}


/**
* Before the game
**/
void RenderAscii::preGame()
{
	this->last_render = st->game_time;
}


/**
* After the game
**/
void RenderAscii::postGame()
{
}


/**
* Clears all colour from a given pixel for a given sprite
**/
void RenderAscii::clearPixel(int x, int y)
{
}


/**
* Free sprite memory
**/
void RenderAscii::freeSprite(SpritePtr sprite)
{
	SDL_FreeSurface(sprite->orig);
	delete(sprite);
}


/**
* Returns sprite width
**/
int RenderAscii::getSpriteWidth(SpritePtr sprite)
{
	return 0;
}

/**
* Returns sprite height
**/
int RenderAscii::getSpriteHeight(SpritePtr sprite)
{
	return 0;
}


/**
* This is the render method for the game
* For this renderer, we actually just dump a little bit of info about the game state
* Every ~ 1000ms
**/
void RenderAscii::render()
{
	if (st->game_time - this->last_render < 1000) return;
	this->last_render = st->game_time;

	float scalex = ((float)st->map->width) / ((float)this->width);
	float scaley = ((float)st->map->height) / ((float)this->height);

	memset(this->buffer, (int)' ', this->width * this->height);
	memset(this->color, 7, this->width * this->height);				// white

	// Entities
	for (list<Entity*>::iterator it = st->entities.begin(); it != st->entities.end(); ++it) {
		Entity *e = (*it);
		btTransform trans = e->getTransform();

		int x = (int)round(trans.getOrigin().getX() / scalex);
		int y = (int)round(trans.getOrigin().getZ() / scaley);

		if (x < 0 || x > this->width) continue;
		if (y < 0 || y > this->height) continue;

		if (e->klass() == WALL) {
			this->buffer[y * this->width + x] = '#';

		} else if (e->klass() == VEHICLE) {
			this->buffer[y * this->width + x] = '%';

		} else if (e->klass() == OBJECT) {
			this->buffer[y * this->width + x] = 'o';

		} else if (e->klass() == PICKUP) {
			this->buffer[y * this->width + x] = '@';

		} else if (e->klass() == UNIT) {
			if (((Unit*)e)->slot == 0) {
				this->buffer[y * this->width + x] = 'N';
				this->color[y * this->width + x] = (int)((Unit*)e)->fac + 1;
			} else {
				this->buffer[y * this->width + x] = 'P';
				this->color[y * this->width + x] = (int)((Unit*)e)->fac + 1;
			}
		}
	}

	printf("\033[2J\033[1;1H\033[40;1m");

	char* ptr = this->buffer;
	Uint8* col = this->color;
	for (int y = this->height; y != 0; --y) {
		for (int x = this->width; x != 0; --x) {
			if (*ptr == ' ') {
				putchar(*ptr);
			} else if (*col > 7) {
				printf("\033[%i;1m%c", 30 + *col, *ptr);
			} else {
				printf("\033[%im%c", 30 + *col, *ptr);
			}
			ptr++; col++;
		}
		if (y != 1) putchar('\n');
	}

	printf("\033[0m\n");
}
