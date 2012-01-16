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
	Mod * mod = new Mod(st, "data/menu");
	
	SpritePtr logo = this->render->loadSprite("logo.png", mod);
	
	this->render->loadFont("orb.otf", mod);
	
	WavefrontObj * bgmesh = loadObj("data/menu/bg.obj");
	SpritePtr bg = this->render->loadSprite("bg.jpg", mod);
	float bg_rot1_pos = -10;
	float bg_rot1_dir = 0.04;
	float bg_rot2_pos = 3;
	float bg_rot2_dir = -0.02;
	
	
	// Maps
	int map = 0;
	vector<string> maps;
	maps.push_back("test");
	maps.push_back("blank");
	maps.push_back("pacman");
	maps.push_back("warfare");
	maps.push_back("arena");
	
	// Gametypes
	int gametype = 0;
	vector<string> gametypes;
	{
		vector<GameType*> * ut = st->mm->getAllGameTypes();
		for (vector<GameType*>::iterator it = ut->begin(); it != ut->end(); it++) {
			gametypes.push_back((*it)->name);
		}
	}
	
	// Viewmodes
	int viewmode = 0;
	vector<string> viewmodes;
	viewmodes.push_back("top");
	viewmodes.push_back("3rd-person");
	viewmodes.push_back("1st-person");
	
	// Unittypes
	int unittype = 0;
	vector<string> unittypes;
	{
		vector<UnitType*> * ut = st->mm->getAllUnitTypes();
		for (vector<UnitType*>::iterator it = ut->begin(); it != ut->end(); it++) {
			if ((*it)->playable) unittypes.push_back((*it)->name);
		}
	}
	
	
	SDL_Event event;
	
	
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
						
						
					case SDLK_e:
						if (viewmode > 0) viewmode--;
						break;
						
					case SDLK_d:
						if (viewmode < ((int) viewmodes.size()) - 1) viewmode++;
						break;
						
						
					case SDLK_r:
						if (unittype > 0) unittype--;
						break;
						
					case SDLK_f:
						if (unittype < ((int) unittypes.size()) - 1) unittype++;
						break;


					case SDLK_l:
					case SDLK_RETURN:
						{
							st->physics->preGame();
							//st->render->enablePhysicsDebug();
							
							// Load map
							Map *m = new Map(st);
							m->load(maps[map], st->render);
							st->curr_map = m;
							
							// Load gametype
							new GameLogic(st);
							GameType *gt = st->mm->getGameType(gametypes[gametype]);
							st->logic->execScript(gt->script);
							
							st->logic->selected_unittype = st->mm->getUnitType(unittypes[unittype]);

							st->client = NULL;
							st->num_local = 1;
							st->local_players[0] = NULL;

							((RenderOpenGL*)st->render)->viewmode = viewmode;
							
							// Begin!
							gameLoop(st, st->render);
						}
						break;
						
					case SDLK_n:
						{
							// Load map
							Map *m = new Map(st);
							m->load("blank", st->render);
							st->curr_map = m;
							
							// Load gametype
							new GameLogic(st);
							//GameType *gt = st->mm->getGameType("boredem");
							//st->logic->execScript(gt->script);
							
							new NetClient(st);
							st->client->bind("localhost", 17778);
							st->client->addmsgJoinReq();
							
							// Begin!
							gameLoop(st, st->render);
						}
						break;
						
					case SDLK_m:
						{
							st->physics->preGame();
							
							// Load map
							Map *m = new Map(st);
							m->load(maps[map], st->render);
							st->curr_map = m;
							
							// Load gametype
							new GameLogic(st);
							GameType *gt = st->mm->getGameType(gametypes[gametype]);
							st->logic->execScript(gt->script);
							
							st->logic->selected_unittype = st->mm->getUnitType(unittypes[unittype]);
							
							st->client = NULL;
							st->num_local = 2;
							st->local_players[0] = NULL;
							st->local_players[1] = NULL;

							((RenderOpenGL*)st->render)->viewmode = viewmode;
							
							// Begin!
							gameLoop(st, st->render);
						}
						break;

					case SDLK_p:
						render->setScreenSize(render->desktop_width, render->desktop_height, true);
						break;


					case SDLK_ESCAPE:
						running = false;
						break;
					
					default: break;
				}
				
			}
		}
		
		
		bg_rot1_pos += bg_rot1_dir;
		if (bg_rot1_pos >= 10.0 or bg_rot1_pos <= -10.0) {
			bg_rot1_dir = 0.0 - bg_rot1_dir;
		}
		
		bg_rot2_pos += bg_rot2_dir;
		if (bg_rot2_pos >= 3.0 or bg_rot2_pos <= -3.0) {
			bg_rot2_dir = 0.0 - bg_rot2_dir;
		}
		
		
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		
		gluPerspective(30.0f, render->virt_width / render->virt_height, 1.0f, 2500.f);
		glScalef (1.0f, -1.0f, 1.0f);
		glTranslatef(0 - (render->virt_width / 2), 0 - (render->virt_height / 2), -1250.0f);
		
		glMatrixMode(GL_MODELVIEW);
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_FOG);
		
		
		glLoadIdentity();
		glTranslatef(500, 500, 0);
		glRotatef(90, 1, 0, 0);
		glRotatef(bg_rot1_pos, 0, 0, 1);
		glRotatef(bg_rot2_pos, 1, 0, 0);
		glScalef(650, 50, 650);
		glBindTexture(GL_TEXTURE_2D, bg->pixels);
		render->renderObj(bgmesh);
		
		
		glLoadIdentity();
		glTranslatef(0, 0, -600);
		render->renderSprite(logo, (render->virt_width - logo->w) / 2, 20);
		
		
		
		int y = 200;
		glLoadIdentity();
		glTranslatef(0, 0, -600);
		glRotatef(5, 0, 1, 0);
		render->renderText("Map:", 20, y);
		glRotatef(-5, 0, 1, 0);
		render->renderText(maps[map], 160, y);
		glRotatef(-5, 0, 1, 0);
		render->renderText("Change with Q and A", 450, y);
		
		y += 30;
		glLoadIdentity();
		glTranslatef(0, 0, -600);
		glRotatef(5, 0, 1, 0);
		render->renderText("Gametype:", 20, y);
		glRotatef(-5, 0, 1, 0);
		render->renderText(gametypes[gametype], 160, y);
		glRotatef(-5, 0, 1, 0);
		render->renderText("Change with W and S", 450, y);
		
		y += 30;
		glLoadIdentity();
		glTranslatef(0, 0, -600);
		glRotatef(5, 0, 1, 0);
		render->renderText("Viewmode:", 20, y);
		glRotatef(-5, 0, 1, 0);
		render->renderText(viewmodes[viewmode], 160, y);
		glRotatef(-5, 0, 1, 0);
		render->renderText("Change with E and D", 450, y);
		
		y += 30;
		glLoadIdentity();
		glTranslatef(0, 0, -600);
		glRotatef(5, 0, 1, 0);
		render->renderText("Unit type:", 20, y);
		glRotatef(-5, 0, 1, 0);
		render->renderText(unittypes[unittype], 160, y);
		glRotatef(-5, 0, 1, 0);
		render->renderText("Change with R and F", 450, y);
		
		glLoadIdentity();
		glTranslatef(0, 0, -600);
		glRotatef(5, 0, 1, 0);
		
		y += 60;
		render->renderText("Start a local game with L", 20, y);
		
		y += 30;
		render->renderText("Start a network game with N (really buggy)", 20, y);
		
		y += 30;
		render->renderText("Start a split-screen game with M (less buggy)", 20, y);

		y += 30;
		render->renderText("Switch to fullscreen with P (do this before anything else)", 20, y);

		y += 60;
		render->renderText("Quit with ESC", 20, y);
		
		
		y = render->virt_height - 30 * 9;
		render->renderText("Controls:", 20, y); y += 30;
		render->renderText("Move", 20, y); render->renderText("W-S-A-D", 230, y); y += 30;
		render->renderText("Look", 20, y); render->renderText("Mouse", 230, y); y += 30;
		render->renderText("Action", 20, y); render->renderText("E", 230, y); y += 30;
		render->renderText("Pick-up", 20, y); render->renderText("Q", 230, y); y += 30;
		render->renderText("Change weapon", 20, y); render->renderText("Scroll-wheel", 230, y); y += 30;
		render->renderText("Fire", 20, y); render->renderText("Left-Click", 230, y); y += 30;
		render->renderText("Melee", 20, y); render->renderText("Right-Click", 230, y); y += 30;
		render->renderText("Special Attack", 20, y); render->renderText("T", 230, y); y += 30;
		
		
		SDL_GL_SwapBuffers();
		
		SDL_Delay(50);
	}
	
}
