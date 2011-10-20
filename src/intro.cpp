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
	this->render = (RenderOpenGL*) st->render;
}


void Intro::doit()
{
	Mod * mod = new Mod(st, "data/intro");
	
	SpritePtr img = this->render->loadSprite("joshcorp.png", mod);
	
	
	Song* sg;
	sg = new Song();
	sg->name = "intro";

	SDL_RWops * rwops = mod->loadRWops("intro.ogg");
	sg->music = Mix_LoadMUS_RW(rwops);

	if (sg->music != NULL) {
		st->audio->playSong(sg);
	}


	SDL_Event event;
	
	glTranslatef(0, 0, 40);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	
	
	
	int start = SDL_GetTicks();
	int time = 0;
	
	this->running = true;
	while (this->running) {
		
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				this->running = false;
			}
		}
		
		time = SDL_GetTicks() - start;
		
		
		if (time > 2000) {
			this->running = false;
			
		} else if (time > 1000) {
			img = this->render->loadSprite("game.png", mod);
			
		} else if (time > 500) {
			img = this->render->loadSprite("sdl.png", mod);
		}
		
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		int x = (render->virt_width - img->w) / 2;
		int y = (render->virt_height - img->h) / 2;
		render->renderSprite(img, x, y);
		
		SDL_GL_SwapBuffers();
		
		SDL_Delay(50);
	}
	
}
