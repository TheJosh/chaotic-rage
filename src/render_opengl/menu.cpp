// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>

#include "gl.h"
#include "menu.h"
#include "render_opengl.h"
#include "animplay.h"

#include "../guichan/guichan.hpp"
#include "../guichan/sdl.hpp"
#include "../guichan/opengl.hpp"
#include "../guichan/opengl/openglsdlimageloader.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../i18n/gettext.h"
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
}


Menu::~Menu()
{
	menuClear();
	delete(this->logo);
	delete(this->bg);
}


/**
* Load stuff from the currently loaded mods into some arrays.
**/
void Menu::loadModBits(UIUpdate* ui)
{
	Mod* mod = GEng()->mm->getSupplOrBase();

	this->gm->loadModBits(ui);

	// Logo
	delete(this->logo);
	this->logo = this->render->loadSprite("menu/logo.png", mod);
	if (!logo) {
		this->logo = this->render->loadSprite("menu/logo.png", GEng()->mm->getBase());
	}

	// Background
	delete(this->bg);
	this->bg = this->render->loadSprite("menu/bg.jpg", mod);
	if (!bg) {
		this->bg = this->render->loadSprite("menu/bg.jpg", GEng()->mm->getBase());
	}

	this->loadMenuItems();
}


/**
* Load main menu items
**/
void Menu::loadMenuItems()
{
	Mod* mod = GEng()->mm->getSupplOrBase();

	this->menuClear();
	int y = render->getHeight() - 60;

	this->menuAdd(_(STRING_MENU_QUIT), 40, y, MC_QUIT);
	y -= 40;

	this->menuAdd(_(STRING_MENU_HELP), 40, y, MC_HELP);
	y -= 40;

	this->menuAdd(_(STRING_MENU_MODS), 40, y, MC_MODS);
	y -= 40;

	this->menuAdd(_(STRING_MENU_SETTINGS), 40, y, MC_SETTINGS);
	y -= 40;

	if (mod->hasArcade()) {
		this->menuAdd(_(STRING_MENU_NETWORK), 40, y, MC_NETWORK);
		y -= 40;

		this->menuAdd(_(STRING_MENU_SPLIT), 40, y, MC_SPLITSCREEN);
		y -= 40;

		this->menuAdd(_(STRING_MENU_SINGLE), 40, y, MC_SINGLEPLAYER);
		y -= 40;
	}

	if (mod->hasCampaign()) {
		this->menuAdd(_(STRING_MENU_CAMPAIGN), 40, y, MC_CAMPAIGN);
	}
}


/**
* Run the menu
* The UIUpdate is for the first-run on the menu, duing initial loading of the menu stuff
**/
void Menu::doit(UIUpdate* ui)
{
	Mod *mod = GEng()->mm->getBase();


	this->input = new gcn::SDLInput();
	this->gui = new gcn::Gui();
	this->gui->setInput(input);
	this->render->initGuichan(gui, mod);
	this->gui_container = new gcn::Container();
	this->gui_container->setDimension(gcn::Rectangle(0, 0, render->real_width, render->real_height));
	this->gui_container->setOpaque(false);
	this->gui->setTop(this->gui_container);

	// This is a hack and leaks memory too
	// TODO: Mod to be able to specify models
	this->model_rot = -10.0f;
	this->model = mod->getAssimpModel("earth.dae");
	this->play = new AnimPlay(this->model);

	this->loadModBits(ui);
	this->setupGLstate();

	// Menu loop
	this->running = true;
	while (this->running) {
		this->updateUI();
	}

	delete(this->input);
	delete(this->gui);
	delete(this->gui_container);
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

	// Projection and view for 3D elements
	render->projection = glm::perspective(45.0f, static_cast<float>(render->real_width) / static_cast<float>(render->real_height), 1.0f, 2500.0f);
	render->view = glm::mat4(1.0f);

	// Assign lights to phong shader
	glUseProgram(render->shaders["phong"]->p());
	glUniform3fv(render->shaders["phong"]->uniform("uLightPos"), 1, glm::value_ptr(glm::vec3(-200.0f, 200.0f, -1550.0f)));		// TODO: This appears to be ignored
	glUniform4fv(render->shaders["phong"]->uniform("uLightColor"), 1, glm::value_ptr(glm::vec4(0.7f, 0.7f, 0.6f, 1.0f)));
	glUniform4fv(render->shaders["phong"]->uniform("uAmbient"), 1, glm::value_ptr(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f)));

	// Begin render
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw background
	glDisable(GL_DEPTH_TEST);
	glUseProgram(render->shaders["basic"]->p());
	glUniformMatrix4fv(render->shaders["basic"]->uniform("uMVP"), 1, GL_FALSE, glm::value_ptr(render->ortho));
	this->render->renderSprite(bg, 0, ((static_cast<float>(render->real_height - render->real_width)) / 2.0f), render->real_width, render->real_width);

	// We need depth for the 3D objects
	glEnable(GL_DEPTH_TEST);

	// Draw an earth
	if (this->model != NULL && this->play != NULL) {
		model_rot += 0.005f;
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(400.0f, -200.0f, -1250.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(250.0f, 250.0f, 250.0f));
		modelMatrix = glm::rotate(modelMatrix, model_rot, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, -20.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		this->render->renderAnimPlay(this->play, modelMatrix);
	}

	// Menu items
	glDisable(GL_DEPTH_TEST);
	this->menuRender();

	// Logo in top-left
	glEnable(GL_BLEND);
	glUseProgram(render->shaders["basic"]->p());
	glUniformMatrix4fv(render->shaders["basic"]->uniform("uMVP"), 1, GL_FALSE, glm::value_ptr(render->ortho));
	this->render->renderSprite(logo, 30, 30);

	// Render guichan and process events
	gui->logic();
	gui->draw();

	SDL_GL_SwapWindow(render->window);
}


/**
* Clear all of the current menu items
**/
void Menu::menuClear()
{
	for (vector<MenuItem*>::iterator it = this->menuitems.begin(); it != this->menuitems.end(); ++it) {
		delete(*it);
		*it = NULL;
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

		this->render->renderText(m->name, m->x1 + 1.0f, m->y1 + 20.0f + 1.0f, 0.1f, 0.1f, 0.1f, 1.0f);

		if (m->hover) {
			this->render->renderText(m->name, m->x1, m->y1 + 20.0f, 161.0f/255.0f, 0.0f, 0.0f, 1.0f);
		} else {
			this->render->renderText(m->name, m->x1, m->y1 + 20.0f, 1.0f, 1.0f, 1.0f, 1.0f);
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

		if (x >= m->x1 && x <= m->x2 && y >= m->y1 && y <= m->y2) {
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

		if (x >= m->x1 && x <= m->x2 && y >= m->y1 && y <= m->y2) {
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
	this->openDialogs.push_back(dialog);
}


/**
* Remove a dialog from the list, removing it from the UI
**/
void Menu::remDialog(Dialog* dialog)
{
	this->gui_container->remove(dialog->getContainer());
	for (vector<Dialog*>::iterator it = openDialogs.begin(); it != openDialogs.end(); ++it) {
		if (*it == dialog) {
			openDialogs.erase(it);
			break;
		}
	}
	delete dialog->getContainer();
	delete dialog;
}


/**
* Remove all dialogs from the menu UI
**/
void Menu::remAllDialogs()
{
	this->gui_container->clear();
	for (vector<Dialog*>::iterator it = openDialogs.begin(); it != openDialogs.end(); ++it) {
		delete(*it);
	}
	this->openDialogs.clear();
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
	this->remAllDialogs();
	this->running = false;
}



/**
* Plays through each game of a campaign
**/
void Menu::startCampaign(Campaign* c, string unittype, GameSettings::ViewMode viewmode, unsigned int num_local)
{
	gm->startCampaign(c, unittype, viewmode, num_local);
	this->setupGLstate();
}


/**
* Starts a game with the specified settings
*
* @param GameSettings::ViewMode viewmode The camera view mode for the local player(s)
* @param int num_local The number of local players; 1 for single, 2+ for splitscreen
* @param int host Set to 1 to host a network game, 0 for a local-only game
**/
void Menu::startGame(MapReg *map, string gametype, string unittype, GameSettings::ViewMode viewmode, unsigned int num_local, bool host, GameSettings* gs)
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
