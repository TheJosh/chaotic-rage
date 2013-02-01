// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"


using namespace std;


Intro::Intro(GameState *st)
{
	this->st = st;
	this->render = st->render;
}


/**
* Load intro assets.
* Has to be before doit()
* Is not part of doit() because that's sometimes run with a different context.
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

	if (img1 == NULL || img2 == NULL || img3 == NULL) {
		reportFatalError("Intro failed to load.  Is the working directory correct?");
	}

	sg = new Song();
	sg->name = "intro";
	SDL_RWops * rwops = mod->loadRWops("intro.ogg");
	sg->music = Mix_LoadMUS_RW(rwops);
}


/**
* Run the intro
**/
void Intro::doit()
{
	if (sg->music != NULL) {
		st->audio->playSong(sg);
	}

	SpritePtr img = this->img1;
	SDL_Event event;
	
	glTranslatef(0, 0, 40);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	
	
	// Ortho mode for the logos
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, this->render->getWidth(), this->render->getHeight(), 0.0f, 0.0f, 10.0f);
	
	
	int start = SDL_GetTicks();
	int time = 0;
	
	bool animdone = false;
	while (!animdone) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				animdone = true;
			}
		}
		
		time = SDL_GetTicks() - start;
		
		if (time > 1500) {
			animdone = true;

		} else if (time > 1000) {
			img = this->img3;
			
		} else if (time > 500) {
			img = this->img2;
		}
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		int x = (this->render->getWidth() - img->w) / 2;
		int y = (this->render->getHeight() - img->h) / 2;
		this->render->renderSprite(img, x, y);
		
		SDL_GL_SwapBuffers();
		SDL_Delay(50);
	}
}
