// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>

#include "gl.h"
#include "menu.h"
#include "render_opengl.h"

#include "../guichan/guichan.hpp"
#include "../guichan/sdl.hpp"
#include "../guichan/opengl.hpp"
#include "../guichan/opengl/openglsdlimageloader.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../rage.h"
#include "../render/sprite.h"
#include "../audio/audio.h"
#include "../game_state.h"
#include "../gui/client_settings.h"
#include "../gui/newcampaign.h"
#include "../gui/newgame.h"
#include "../gui/network.h"
#include "../gui/mods.h"
#include "../gui/controls.h"
#include "../mod/campaign.h"
#include "../mod/mod.h"
#include "../util/obj.h"
#include "../util/serverconfig.h"
#include "../mod/mod_manager.h"
#include "../map/map.h"
#include "../game_manager.h"
#include "../game_engine.h"

using namespace std;


Menu::Menu(GameState *st, GameManager *gm)
{
	this->st = st;
	this->gm = gm;
	this->render = (RenderOpenGL*) GEng()->render;
	this->logo = NULL;
	this->bg = NULL;
	this->bgmesh = NULL;
}


/**
* Load stuff from the currently loaded mods into some arrays.
**/
void Menu::loadModBits(UIUpdate* ui)
{
	Mod* mod = GEng()->mm->getSupplOrBase();

	this->gm->loadModBits(ui);

	if (ui) ui->updateUI();

	// Logo
	delete(this->logo);
	this->logo = this->render->loadSprite("menu/logo.png", mod);
	if (!logo) {
		this->logo = this->render->loadSprite("menu/logo.png", GEng()->mm->getBase());
	}

	if (ui) ui->updateUI();

	// Background
	delete(this->bg);
	this->bg = this->render->loadSprite("menu/bg.jpg", mod);
	if (!bg) {
		this->bg = this->render->loadSprite("menu/bg.jpg", GEng()->mm->getBase());
	}

	if (ui) ui->updateUI();

	// Main menu items
	this->menuClear();
	int y = render->getHeight() - 60;
	
	this->menuAdd("Quit", 40, y, MC_QUIT);
	y -= 40;
	
	this->menuAdd("Help", 40, y, MC_HELP);
	y -= 40;
	
	this->menuAdd("Mods", 40, y, MC_MODS);
	y -= 40;
	
	this->menuAdd("Settings", 40, y, MC_SETTINGS);
	y -= 40;
	
	if (mod->hasArcade()) {
		this->menuAdd("Network Game", 40, y, MC_NETWORK);
		y -= 40;
		
		this->menuAdd("Split Screen", 40, y, MC_SPLITSCREEN);
		y -= 40;
		
		this->menuAdd("Single Player", 40, y, MC_SINGLEPLAYER);
		y -= 40;
	}
	
	if (mod->hasCampaign()) {
		this->menuAdd("Campaign", 40, y, MC_CAMPAIGN);
	}
}


/**
* Create the "background" mesh
* We save it in a WavefrontObj.
**/
void Menu::createBGmesh()
{
	if (bgmesh != NULL) return;
	
	Vertex v;
	TexUV t;
	Face f;
	
	bgmesh = new WavefrontObj();
	v.y = 0.0;
	
	v.x = -1.0f; v.z = -1.0f; t.x = 0.0f; t.y = 0.0f;
	bgmesh->vertexes.push_back(v);
	bgmesh->texuvs.push_back(t);
	
	v.x = -1.0f; v.z = 1.0f; t.x = 0.0f; t.y = 1.0f;
	bgmesh->vertexes.push_back(v);
	bgmesh->texuvs.push_back(t);
	
	v.x = 1.0f; v.z = 1.0f; t.x = 1.0f; t.y = 1.0f;
	bgmesh->vertexes.push_back(v);
	bgmesh->texuvs.push_back(t);
	
	v.x = 1.0f; v.z = -1.0f; t.x = 1.0f; t.y = 0.0f;
	bgmesh->vertexes.push_back(v);
	bgmesh->texuvs.push_back(t);
	
	v.x = 0.0f; v.y = 1.0f; v.z = 0.0f;
	bgmesh->normals.push_back(v);
	
	f.v1 = 1; f.v2 = 2; f.v3 = 3;
	f.t1 = 1; f.t2 = 2; f.t3 = 3;
	f.n1 = 1; f.n2 = 1; f.n3 = 1;
	bgmesh->faces.push_back(f);
	
	f.v1 = 1; f.v2 = 3; f.v3 = 4;
	f.t1 = 1; f.t2 = 3; f.t3 = 4;
	f.n1 = 1; f.n2 = 1; f.n3 = 1;
	bgmesh->faces.push_back(f);
}


/**
* Run the menu
* The UIUpdate is for the first-run on the menu, duing initial loading of the menu stuff
**/
void Menu::doit(UIUpdate* ui)
{
	Mod *mod = GEng()->mm->getBase();
	
	
	// A few bits always load off base
	this->render->loadFont("orb.otf", mod);
	
	if (ui) ui->updateUI();

	this->input = new gcn::SDLInput();
	this->gui = new gcn::Gui();
	this->gui->setInput(input);
	this->render->initGuichan(gui, mod);
	this->gui_container = new gcn::Container();
	this->gui_container->setDimension(gcn::Rectangle(0, 0, render->real_width, render->real_height));
	this->gui_container->setOpaque(false);
	this->gui->setTop(this->gui_container);
	
	if (ui) ui->updateUI();

	this->createBGmesh();
	this->bg_rot1_pos = -10.0f;
	this->bg_rot1_dir = 0.006f;
	this->bg_rot2_pos = 3.0f;
	this->bg_rot2_dir = -0.004f;
	
	if (ui) ui->updateUI();

	this->loadModBits(ui);
	this->setupGLstate();
	
	// Menu loop
	this->running = true;
	while (this->running) {
		this->updateUI();
	}
}


/**
* Set the OpenGL state just the way we like it.
**/
void Menu::setupGLstate()
{
	this->render->mainViewport(0, 0);
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	// Ortho for menu stuff
	render->ortho = glm::ortho<float>(0.0f, (float)render->real_width, (float)render->real_height, 0.0f, 0.0f, 10.0f);
	
	GEng()->setMouseGrab(false);
}


/**
* Update the UI for the menu
**/
void Menu::updateUI()
{
	int mousex, mousey;
	SDL_Event event;
	
	// Menu hover
	SDL_GetMouseState(&mousex, &mousey);
	this->menuHover(mousex, mousey);
	
	// Events
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
					this->remAllDialogs();
					break;

				case SDLK_PRINTSCREEN:
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
		case MC_SETTINGS: this->doSettings(); break;
		case MC_MODS: this->doMods(); break;
		case MC_HELP: this->doHelp(); break;
		case MC_QUIT: this->doQuit(); break;
		default: break;
	}
	
	// Background animation
	bg_rot1_pos += bg_rot1_dir;
	if (bg_rot1_pos >= 10.0f or bg_rot1_pos <= -10.0f) {
		bg_rot1_dir = 0.0f - bg_rot1_dir;
	}
	
	bg_rot2_pos += bg_rot2_dir;
	if (bg_rot2_pos >= 3.0f or bg_rot2_pos <= -3.0f) {
		bg_rot2_dir = 0.0f - bg_rot2_dir;
	}
	
	glm::mat4 proj = glm::perspective(30.0f, (float)(render->virt_width / render->virt_height), 1.0f, 2500.f);
	proj = glm::scale(proj, glm::vec3(1.0f, -1.0f, 1.0f));
	
	glm::mat4 bgMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1250.0f));
	bgMatrix = glm::rotate(bgMatrix, 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	bgMatrix = glm::rotate(bgMatrix, bg_rot1_pos, glm::vec3(0.0f, 0.0f, 1.0f));
	bgMatrix = glm::rotate(bgMatrix, bg_rot2_pos, glm::vec3(1.0f, 0.0f, 0.0f));
	bgMatrix = glm::scale(bgMatrix, glm::vec3(650.0f, 50.0f, 650.0f));
	
	
	// Begin render
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glBindTexture(GL_TEXTURE_2D, bg->pixels);
	this->render->renderObj(bgmesh, proj * bgMatrix);
	
	// Menu items
	this->menuRender();
	
	// Logo in top-left
	glUseProgram(render->shaders["basic"]->p());
	glUniformMatrix4fv(render->shaders["basic"]->uniform("uMVP"), 1, GL_FALSE, glm::value_ptr(render->ortho));
	this->render->renderSprite(logo, 40, 40);
	
	// If a guichan dialog is set, render it and process events
	//if (this->dialog != NULL) {
		gui->logic();
		gui->draw();
	//}
	
	SDL_GL_SwapWindow(render->window);
}


/**
* Clear all of the current menu items
**/
void Menu::menuClear()
{
	for (vector<MenuItem*>::iterator it = this->menuitems.begin(); it != this->menuitems.end(); ++it) {
		delete(*it);
	}
	this->menuitems.clear();
	this->menuitems.reserve(7);
}


/**
* Adds a menu item to the main menu
**/
void Menu::menuAdd(string name, int x, int y, MenuCommand cmd)
{
	MenuItem * nu = new MenuItem();
	this->menuitems.push_back(nu);
	
	nu->x1 = static_cast<float>(x);
	nu->x2 = static_cast<float>(x) + 200.0f;
	nu->y1 = static_cast<float>(y);
	nu->y2 = static_cast<float>(y) + 20.0f;
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
		
		this->render->renderText(m->name, m->x1 + 1.0f, m->y1 + 20.0f + 1.0f, 0.1f, 0.1f, 0.1f);
		
		if (m->hover) {
			this->render->renderText(m->name, m->x1, m->y1 + 20.0f, 161.0f/255.0f, 0.0f, 0.0f);
		} else {
			this->render->renderText(m->name, m->x1, m->y1 + 20.0f, 1.0f, 1.0f, 1.0f);
		}
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
* Add a dialog to the list, making it visible in the UI
**/
void Menu::addDialog(Dialog* dialog)
{
	dialog->m = this;
	dialog->gm = this->gm;
	dialog->setup();
	
	gcn::Container* c = dialog->getContainer();
	
	c->setPosition((this->render->real_width - c->getWidth()) / 2, (this->render->real_height - c->getHeight()) / 2);
	c->setBaseColor(gcn::Color(150, 150, 150, 225));
	
	this->gui_container->add(c);
}


/**
* Remove a dialog to the list, removing it from the UI
**/
void Menu::remDialog(Dialog* dialog)
{
	this->gui_container->remove(dialog->getContainer());
	delete dialog->getContainer();
	delete dialog;
}


/**
* Remove all dialogs from the menu UI
**/
void Menu::remAllDialogs()
{
	// TODO: delete dialogs
	this->gui_container->clear();
}



void Menu::doCampaign()
{
	this->remAllDialogs();
	this->addDialog(new DialogNewCampaign(1, GEng()->mm->getSupplOrBase()));
}

void Menu::doSingleplayer()
{
	this->remAllDialogs();
	this->addDialog(new DialogNewGame(1));
}

void Menu::doSplitscreen()
{
	this->remAllDialogs();
	this->addDialog(new DialogNewGame(2));
}

void Menu::doNetwork()
{
	this->remAllDialogs();
	this->addDialog(new DialogNetJoin());
}

void Menu::doSettings()
{
	this->remAllDialogs();
	this->addDialog(new DialogClientSettings(this->st));
}

void Menu::doMods()
{
	this->remAllDialogs();
	this->addDialog(new DialogMods(this->st));
}

void Menu::doHelp()
{
	this->remAllDialogs();
	this->addDialog(new DialogControls());
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
	gm->startCampaign(c, unittype, viewmode, num_local);
	this->setupGLstate();
}


/**
* Starts a game with the specified settings
*
* @param int viewmode The camera view mode for the local player(s)
* @param int num_local The number of local players; 1 for single, 2+ for splitscreen
* @param int host Set to 1 to host a network game, 0 for a local-only game
**/
void Menu::startGame(MapReg *map, string gametype, string unittype, int viewmode, unsigned int num_local, bool host, GameSettings* gs)
{
	gm->startGame(map, gametype, unittype, viewmode, num_local, host, gs);
	this->setupGLstate();
}

	
/**
* Join a network game
**/
void Menu::networkJoin(string host)
{
	gm->networkJoin(host, this);
	this->setupGLstate();
}

