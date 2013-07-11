// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "intro.h"
#include "rage.h"
#include "render/render.h"
#include "mod/mod.h"
#include "mod/song.h"
#include "audio/audio.h"
#include "gamestate.h"


using namespace std;


Intro::Intro(GameState *st)
{
	this->st = st;
	this->render = st->render;
}

Intro::~Intro()
{
	delete(img1);
	delete(img2);
	delete(img3);
}


/**
* Load intro assets.
* Has to be before doit()
**/
void Intro::load()
{
	Mod * mod = new Mod(st, "data/intro");
	if (!mod) {
		reportFatalError("Intro failed to load. Is the working directory correct?");
	}

	img1 = this->render->loadSprite("joshcorp.png", mod);
	img2 = this->render->loadSprite("sdl.png", mod);
	img3 = this->render->loadSprite("game.png", mod);
	text = this->render->loadSprite("loading.png", mod);

	if (img1 == NULL || img2 == NULL || img3 == NULL) {
		reportFatalError("Intro failed to load.  Is the working directory correct?");
	}

	sg = new Song();
	sg->name = "intro";
	SDL_RWops * rwops = mod->loadRWops("intro.ogg");
	sg->music = Mix_LoadMUS_RW(rwops);
}


/**
* Prepare to run the intro
**/
void Intro::doit()
{
	if (sg->music != NULL) {
		st->audio->playSong(sg);
	}

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, this->render->getWidth(), this->render->getHeight(), 0.0f, 0.0f, 10.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	start = SDL_GetTicks();
	lasttime = 0;

	this->updateUI();
}


/**
* This is called at regular intervals by the module loading code
**/
void Intro::updateUI()
{
	int time = SDL_GetTicks() - start;
	SpritePtr img;
	
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			// TODO: Think up a way to inform the loader to abort
		}
	}

	// Update the image
	if (time > 1000) {
		img = this->img3;
	} else if (time > 500) {
		img = this->img2;
	} else {
		img = this->img1;
	}
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render img
	int x = (this->render->getWidth() - img->w) / 2;
	int y = (this->render->getHeight() - img->h) / 2;
	this->render->renderSprite(img, x, y);

	// Flashing message
	int frame = (int)floor(time / 500.0f);
	if (frame % 2 == 0) {
		x = (this->render->getWidth() - text->w) / 2;
		y = this->render->getHeight() - text->h - 50;
		this->render->renderSprite(text, x, y);
	}

	SDL_GL_SwapBuffers();
}


