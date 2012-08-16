// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <guichan/opengl.hpp>
#include <guichan/opengl/openglsdlimageloader.hpp>

#include "rage.h"
#include "menu.h"


using namespace std;


Menu::Menu(GameState *st)
{
	this->st = st;
	this->render = (RenderOpenGL*) st->render;
	this->dialog = NULL;
}


void Menu::doit()
{
	Mod * mod = new Mod(st, "data/menu");
	
	SpritePtr logo = this->render->loadSprite("logo.png", mod);
	
	this->render->loadFont("orb.otf", mod);
	
	WavefrontObj * bgmesh = loadObj("data/menu/bg.obj");
	SpritePtr bg = this->render->loadSprite("bg.jpg", mod);
	float bg_rot1_pos = -10;
	float bg_rot1_dir = 0.006;
	float bg_rot2_pos = 3;
	float bg_rot2_dir = -0.004;
	
	
	// Maps
	maps.push_back("test");
	
	// Gametypes
	{
		vector<GameType*> * ut = st->mm->getAllGameTypes();
		for (vector<GameType*>::iterator it = ut->begin(); it != ut->end(); it++) {
			gametypes.push_back((*it)->name);
		}
	}
	
	// Viewmodes
	viewmodes.push_back("Behind player");
	viewmodes.push_back("Above player");
	
	// Unittypes
	{
		vector<UnitType*> * ut = st->mm->getAllUnitTypes();
		for (vector<UnitType*>::iterator it = ut->begin(); it != ut->end(); it++) {
			if ((*it)->playable) unittypes.push_back((*it)->name);
		}
	}
	
	
	int mousex, mousey;
	SDL_Event event;
	
	
	int y = render->real_height - (40 * 6) - 20;
	
	this->menuAdd("Single Player", 40, y, MC_SINGLEPLAYER);
	y += 40;
	
	this->menuAdd("Split Screen", 40, y, MC_SPLITSCREEN);
	y += 40;
	
	this->menuAdd("Network Game", 40, y, MC_NETWORK);
	y += 40;
	
	this->menuAdd("Settings", 40, y, MC_SETTINGS);
	y += 40;
	
	this->menuAdd("Controls", 40, y, MC_CONTROLS);
	y += 40;
	
	this->menuAdd("Quit", 40, y, MC_QUIT);
	
	
	

	gcn::SDLInput* input;
	input = new gcn::SDLInput();

	this->gui = new gcn::Gui();
	this->gui->setInput(input);
	this->render->initGuichan(gui, mod);
	
	
	this->running = true;
	while (this->running) {
		
		SDL_GetMouseState(&mousex, &mousey);
		
		MenuCommand cmd = MC_NOTHING;
		
		
		while (SDL_PollEvent(&event)) {

			if (event.type == SDL_QUIT) {
				cmd = MC_QUIT;
				
			} else if (event.type == SDL_MOUSEBUTTONUP) {
				cmd = this->menuClick(mousex, mousey);
				
			} else if (event.type == SDL_KEYDOWN) {
				// Key press
				switch (event.key.keysym.sym) {
					case SDLK_p:
						render->setScreenSize(render->desktop_width, render->desktop_height, true, 0);
						render->initGuichan(gui, mod);
						break;

					case SDLK_ESCAPE:
						if (this->dialog != NULL) {
							this->setDialog(NULL);
						} else {
							cmd = MC_QUIT;
						}
						break;

					case SDLK_PRINT:
						{
							string filename = getUserDataDir();
							filename.append("menu_screenshot.bmp");
							render->saveScreenshot(filename);
						}
						break;

					default: break;
				}
				
			}

			input->pushInput(event);
		}
		
		
		// Handle main menu commands
		switch (cmd) {
			case MC_SINGLEPLAYER: this->doSingleplayer(); break;
			case MC_SPLITSCREEN: this->doSplitscreen(); break;
			case MC_NETWORK: this->doNetwork(); break;
			case MC_SETTINGS: this->doSettings(); break;
			case MC_CONTROLS: this->doControls(); break;
			case MC_QUIT: this->doQuit(); break;
			default: break;
		}
		

		// Background animation
		bg_rot1_pos += bg_rot1_dir;
		if (bg_rot1_pos >= 10.0 or bg_rot1_pos <= -10.0) {
			bg_rot1_dir = 0.0 - bg_rot1_dir;
		}
		
		bg_rot2_pos += bg_rot2_dir;
		if (bg_rot2_pos >= 3.0 or bg_rot2_pos <= -3.0) {
			bg_rot2_dir = 0.0 - bg_rot2_dir;
		}
		
		
		
		// Set up everything for render
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_FOG);
		glDisable(GL_MULTISAMPLE);


		// Perspective mode for the background
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		
		gluPerspective(30.0f, render->virt_width / render->virt_height, 1.0f, 2500.f);
		glScalef (1.0f, -1.0f, 1.0f);
		glTranslatef(0 - (render->virt_width / 2), 0 - (render->virt_height / 2), -1250.0f);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef((render->virt_width / 2), (render->virt_height / 2), 0);
		glRotatef(90, 1, 0, 0);
		glRotatef(bg_rot1_pos, 0, 0, 1);
		glRotatef(bg_rot2_pos, 1, 0, 0);
		glScalef(650, 50, 650);
		glBindTexture(GL_TEXTURE_2D, bg->pixels);
		render->renderObj(bgmesh);
		
		
		// Ortho mode for the logo and main menu
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0f, render->real_width, render->real_height, 0.0f, 0.0f, 10.0f);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glEnable(GL_BLEND);
		render->renderSprite(logo, 40, 40);

		this->menuHover(mousex, mousey);
		this->menuRender();


		// If a guichan dialog is set, render it and process events
		if (this->dialog != NULL) {
			gui->logic();
			gui->draw();
		}


		SDL_GL_SwapBuffers();
	}
	
}


/**
* Adds a menu item to the main menu
**/
void Menu::menuAdd(string name, int x, int y, MenuCommand cmd)
{
	MenuItem * nu = new MenuItem();
	this->menuitems.push_back(nu);
	
	nu->x1 = x;
	nu->x2 = x + 200;
	nu->y1 = y;
	nu->y2 = y + 20;
	nu->name = name;
	nu->cmd = cmd;
	nu->hover = false;
}


/**
* Renders the main menu
**/
void Menu::menuRender()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	
	for (unsigned int i = 0; i < this->menuitems.size(); i++) {
		MenuItem * m = this->menuitems.at(i);
		
		render->renderText(m->name, m->x1 + 1, m->y1 + 20 + 1, 0.1, 0.1, 0.1);
		
		if (m->hover) {
			render->renderText(m->name, m->x1, m->y1 + 20, 161.0/255.0, 0.0, 0.0);
		} else {
			render->renderText(m->name, m->x1, m->y1 + 20, 1.0, 1.0, 1.0);
		}
		
		glColor3f(1, 1, 1);
	}
}


/**
* Looks for mouse hovers on the main menu
**/
void Menu::menuHover(int x, int y)
{
	for (unsigned int i = 0; i < this->menuitems.size(); i++) {
		MenuItem * m = this->menuitems.at(i);
		
		if (x >= m->x1 and x <= m->x2 and y >= m->y1 and y <= m->y2) {
			m->hover = true;
		} else {
			m->hover = false;
		}
	}
}


/**
* Handles mouse clicks on the main menu, returning the appropriate MenuCommand
**/
MenuCommand Menu::menuClick(int x, int y)
{
	for (unsigned int i = 0; i < this->menuitems.size(); i++) {
		MenuItem * m = this->menuitems.at(i);
		
		if (x >= m->x1 and x <= m->x2 and y >= m->y1 and y <= m->y2) {
			return m->cmd;
		}
	}
	
	return MC_NOTHING;
}



/**
* Sets the current menu dialog, or NULL to hide the dialog
**/
void Menu::setDialog(Dialog * dialog)
{
	this->dialog = dialog;

	if (this->dialog != NULL) {
		dialog->m = this;
		gcn::Container * c = dialog->setup();
		c->setPosition((this->render->real_width - c->getWidth()) / 2, (this->render->real_height - c->getHeight()) / 2);
		c->setBaseColor(gcn::Color(150, 150, 150, 200));
		gui->setTop(c);
	}
}



void Menu::doSingleplayer()
{
	this->setDialog(new DialogNewGame(1));
}

void Menu::doSplitscreen()
{
	this->setDialog(new DialogNewGame(2));
}

void Menu::doNetwork()
{
	this->setDialog(new DialogNull());
}

void Menu::doSettings()
{
	this->setDialog(new DialogNull());
}

void Menu::doControls()
{
	this->setDialog(new DialogControls());
}

void Menu::doQuit()
{
	this->running = false;
}



/**
* Starts a game with the specified settings
**/
void Menu::startGame(string map, string gametype, string unittype, int viewmode, int num_local)
{
	st->physics->init();
	
	// Load map
	Map *m = new Map(st);
	m->load(map, st->render);
	st->curr_map = m;
	
	// Load gametype
	new GameLogic(st);
	GameType *gt = st->mm->getGameType(gametype);
	st->logic->selected_unittype = st->mm->getUnitType(unittype);
	st->logic->execScript(gt->script);
	
	// Reset client variables
	st->client = NULL;
	st->num_local = num_local;
	for (int i = 0; i < num_local; i++) {
		st->local_players[i] = NULL;
	}

	((RenderOpenGL*)st->render)->viewmode = viewmode;
	
	st->physics->preGame();
	
	// Begin!
	gameLoop(st, st->render);
}


