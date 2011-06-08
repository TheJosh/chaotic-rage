// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"


using namespace std;


Menu::Menu(GameState *st)
{
	this->st = st;
	this->render = (RenderOpenGL*) st->render;
}


void Menu::doit()
{
	int map = 0;
	vector<string> maps;
	maps.push_back("test");
	maps.push_back("blank");
	
	int gametype = 0;
	vector<string> gametypes;
	gametypes.push_back("zombies");
	
	SDL_Event event;
	
	glTranslatef(0, 0, 40);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	
	
	this->running = true;
	while (this->running) {
		
		
		while (SDL_PollEvent(&event)) {
			
			if (event.type == SDL_QUIT) {
				this->running = false;
				
			} else if (event.type == SDL_KEYDOWN) {
				// Key press
				switch (event.key.keysym.sym) {
					case SDLK_q:
						if (map > 0) map--;
						break;
						
					case SDLK_a:
						if (map < ((int) maps.size()) - 1) map++;
						break;
						
					case SDLK_w:
						if (gametype > 0) gametype--;
						break;
						
					case SDLK_s:
						if (gametype < ((int) gametypes.size()) - 1) gametype++;
						break;
						
					case SDLK_g:
						{
							// Load map
							Map *m = new Map(st);
							m->load(maps[map], st->render);
							st->curr_map = m;
							
							// Load gametype
							new GameLogic(st);
							GameType *gt = st->getMod(0)->getGameType(gametypes[gametype]);
							st->logic->execScript(gt->script);
							
							// Begin!
							gameLoop(st, st->render);
						}
						break;
						
					case SDLK_ESCAPE:
						running = false;
						break;
					
					default: break;
				}
				
			}
		}
		
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		render->renderText("Map:", 20, 36);
		render->renderText(maps[map], 160, 36);
		render->renderText("Change with Q and A", 450, 36);
		
		render->renderText("Gametype:", 20, 60);
		render->renderText(gametypes[gametype], 160, 60);
		render->renderText("Change with W and S", 450, 60);
		
		render->renderText("Start game with G, quit with ESC", 20, 100);
		
		SDL_GL_SwapBuffers();
	}
	
}
