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
	this->mm = st->mm;
	this->render = (Render3D*) st->render;
	this->dialog = NULL;
	this->mapreg = NULL;
	this->logo = NULL;
	this->bg = NULL;
}


/**
* Load stuff from the currently loaded mods into some arrays.
**/
void Menu::loadModBits()
{
	Mod* mod = st->mm->getSupplOrBase();
	
	// Logo
	delete(this->logo);
	this->logo = this->render->loadSprite("menu/logo.png", mod);
	if (!logo) {
		this->logo = this->render->loadSprite("menu/logo.png", st->mm->getBase());
	}
	
	// Background
	delete(this->bg);
	this->bg = this->render->loadSprite("menu/bg.jpg", mod);
	if (!bg) {
		this->bg = this->render->loadSprite("menu/bg.jpg", st->mm->getBase());
	}
		
	// Load maps
	delete(this->mapreg);
	this->mapreg = new MapRegistry();
	this->mapreg->find(mod);
	this->mapreg->find("maps");

	// Gametypes
	this->gametypes.clear();
	{
		vector<GameType*> * ut = st->mm->getAllGameTypes();
		for (vector<GameType*>::iterator it = ut->begin(); it != ut->end(); it++) {
			this->gametypes.push_back((*it)->name);
		}
		delete(ut);
	}
	
	// Viewmodes
	this->viewmodes.clear();
	this->viewmodes.push_back("Behind player");
	this->viewmodes.push_back("Above player");
	
	// Unittypes
	this->unittypes.clear();
	{
		vector<UnitType*> * ut = st->mm->getAllUnitTypes();
		for (vector<UnitType*>::iterator it = ut->begin(); it != ut->end(); it++) {
			if ((*it)->playable) this->unittypes.push_back((*it)->name);
		}
		delete(ut);
	}
}


void Menu::doit()
{
	Mod *mod = st->mm->getBase();
	
	
	// A few bits always load off base
	this->render->loadFont("orb.otf", mod);
	
	gcn::SDLInput* input;
	input = new gcn::SDLInput();
	
	this->gui = new gcn::Gui();
	this->gui->setInput(input);
	this->render->initGuichan(gui, mod);
	
	WavefrontObj * bgmesh = loadObj("data/cr/menu/bg.obj");
	float bg_rot1_pos = -10.0f;
	float bg_rot1_dir = 0.006f;
	float bg_rot2_pos = 3.0f;
	float bg_rot2_dir = -0.004f;
	

	this->loadModBits();
	
	
	// Main menu items
	int y = render->getHeight() - (40 * 7) - 20;
	
	this->menuAdd("Campaign", 40, y, MC_CAMPAIGN);
	y += 40;

	this->menuAdd("Single Player", 40, y, MC_SINGLEPLAYER);
	y += 40;
	
	this->menuAdd("Split Screen", 40, y, MC_SPLITSCREEN);
	y += 40;
	
	this->menuAdd("Network Game", 40, y, MC_NETWORK);
	y += 40;
	
	this->menuAdd("Mods", 40, y, MC_MODS);
	y += 40;
	
	this->menuAdd("Help", 40, y, MC_HELP);
	y += 40;
	
	this->menuAdd("Quit", 40, y, MC_QUIT);
	
	
	// Menu loop
	int mousex, mousey;
	SDL_Event event;
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
			case MC_CAMPAIGN: this->doCampaign(); break;
			case MC_SINGLEPLAYER: this->doSingleplayer(); break;
			case MC_SPLITSCREEN: this->doSplitscreen(); break;
			case MC_NETWORK: this->doNetwork(); break;
			case MC_MODS: this->doMods(); break;
			case MC_HELP: this->doHelp(); break;
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
		render->preVBOrender();
		render->renderObj(bgmesh);
		render->postVBOrender();

		
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



void Menu::doCampaign()
{
	this->setDialog(new DialogNewCampaign(1, this->mm->getSupplOrBase()));
}

void Menu::doSingleplayer()
{
	this->setDialog(new DialogNewGame(1, this->st->mm));
}

void Menu::doSplitscreen()
{
	this->setDialog(new DialogNewGame(2, this->st->mm));
}

void Menu::doNetwork()
{
	this->setDialog(new DialogNetJoin());
}

void Menu::doMods()
{
	this->setDialog(new DialogMods(this->st, this->st->mm));
}

void Menu::doHelp()
{
	this->setDialog(new DialogControls());
}

void Menu::doQuit()
{
	this->running = false;
}



/**
* Plays through each game of a campaign
**/
void Menu::startCampaign(Campaign* c, string unittype, int viewmode, unsigned int num_local)
{
	for (vector<CampaignStage*>::iterator it = c->stages->begin(); it != c->stages->end();) {
		CampaignStage* stage = *it;

		MapReg* m = mapreg->get(stage->map);
		if (m == NULL) return;

		this->startGame(m, stage->gametype, unittype, 0, 1);

		// TODO: Some way for a gametype to specify if the player advances to the next stage or not
		it++;
	}
}


/**
* Starts a game with the specified settings
**/
void Menu::startGame(MapReg *map, string gametype, string unittype, int viewmode, unsigned int num_local)
{
	st->physics->init();
	
	// Load map
	Map *m = new Map(st);
	m->load(map->getName(), st->render, map->getMod());
	st->curr_map = m;
	
	// Load gametype
	new GameLogic(st);
	GameType *gt = st->mm->getGameType(gametype);
	st->logic->selected_unittype = st->mm->getUnitType(unittype);
	st->logic->execScript(gt->script);
	
	// Reset client variables
	st->client = NULL;
	st->num_local = num_local;
	
	// Create players in GameState.
	for (unsigned int i = 0; i < num_local; i++) {
		st->local_players[i] = new PlayerState(st);
		st->local_players[i]->slot = i + 1;
	}

	st->render->viewmode = viewmode;
	
	st->physics->preGame();
	st->curr_map->preGame();

	// Begin!
	gameLoop(st, st->render);
}

	
/**
* Join a network game
**/
void Menu::networkJoin(string host)
{
	if (st->client) delete (st->client);
	new NetClient(st);

	st->physics->init();

	// Try to join the server
	NetGameinfo *gameinfo = st->client->attemptJoinGame(host, 17778);
	if (gameinfo == NULL) {
		delete (st->client);
		st->client = NULL;
		displayMessageBox("Unable to connect to server " + host);
		return;
	}

	// Load map
	Map *m = new Map(st);
	m->load(gameinfo->map, st->render, NULL);
	st->curr_map = m;
	
	// Load gametype
	new GameLogic(st);
	GameType *gt = st->mm->getGameType("boredem");
	st->logic->selected_unittype = st->mm->getUnitType("robot");
	st->logic->execScript(gt->script);
	
	// Create players in GameState.
	st->num_local = 1;
	for (unsigned int i = 0; i < st->num_local; i++) {
		st->local_players[i] = new PlayerState(st);
		st->local_players[i]->slot = 0;		// allocated by the server
	}

	st->render->viewmode = 0;
	
	// Download the gamestate
	// When this is done, a final message is sent to tell the server we are done.
	bool gotstate = st->client->downloadGameState();
	if (! gotstate) {
		st->audio->stopAll();
		st->clear();
		delete (st->client);
		st->client = NULL;
		displayMessageBox("Unable to download intial game state from server " + host);
		return;
	}

	st->physics->preGame();
	st->curr_map->preGame();

	// Begin!
	gameLoop(st, st->render);
	
	delete (st->client);
	st->client = NULL;
}
