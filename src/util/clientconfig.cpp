// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <string>
#include <confuse.h>
#include "../rage.h"
#include "../game_state.h"
#include "../game_engine.h"
#include "../render_opengl/render_opengl.h"
#include "../render_opengl/render_opengl_settings.h"
#include "../render/render_debug.h"
#include "../render/render_null.h"
#include "../render/render_ascii.h"
#include "../audio/audio_sdlmixer.h"
#include "../audio/audio_null.h"
#include "../mod/mod_manager.h"
#include "../physics_bullet.h"
#include "clientconfig.h"
#include "cmdline.h"


using namespace std;


static cfg_opt_t config_opts[] =
{
	CFG_INT((char*) "gl-msaa", 1, CFGF_NONE),
	CFG_INT((char*) "gl-tex-filter", 4, CFGF_NONE),

	CFG_BOOL((char*) "fullscreen", cfg_false, CFGF_NONE),

	CFG_STR((char*) "lang", 0, CFGF_NONE),

	// Deprecated; only in place to not break existing confings
	CFG_INT((char*) "render", 0, CFGF_NONE),

	CFG_END()
};



/**
* Load the config
* Then save again straight away, to fill the file with defaults
**/
ClientConfig::ClientConfig()
{
	this->load();
}


/**
* Loads the client config
**/
void ClientConfig::load()
{
	cfg_t *cfg;
	string filename = getUserDataDir().append("client.conf");

	// Parse config
	cfg = cfg_init(config_opts, CFGF_NONE);
	cfg_parse(cfg, filename.c_str());

	// OpenGL
	this->gl = new RenderOpenGLSettings();
	this->gl->msaa = cfg_getint(cfg, "gl-msaa");
	this->gl->tex_filter = cfg_getint(cfg, "gl-tex-filter");

	// Fullscreen
	this->fullscreen = cfg_getbool(cfg, "fullscreen");

	// Language
	char* tmp = cfg_getstr(cfg, "lang");
	if (tmp != NULL) {
		this->lang = std::string(tmp);
	} else {
		this->lang = "en";
	}

	cfg_free(cfg);
}


/**
* Save the config
**/
void ClientConfig::save()
{
	string filename = getUserDataDir().append("client.conf");

	FILE * fp = fopen(filename.c_str(), "w");
	if (fp == NULL) {
		return;
	}

	// TODO: Save config _properly_
	// Okay so the correct way is the libConfuse cfg_print function
	// but it wasn't working, and I'm getting too tired to keep trying
	fprintf(fp, "# Config file for CR\n");
	fprintf(fp, "gl-msaa = %i\n", this->gl->msaa);
	fprintf(fp, "gl-tex-filter = %i\n", this->gl->tex_filter);
	fprintf(fp, "fullscreen = %s\n", (this->fullscreen ? "true" : "false"));
	fprintf(fp, "lang = %s\n", this->lang.c_str());

	fclose(fp);
}


/**
* Init rendering engine based on config options
**/
void ClientConfig::initRender(GameState *st)
{
	if (GEng()->cmdline->render_class == "debug") {
		GEng()->render = new RenderDebug(st);

	} else if (GEng()->cmdline->render_class == "null") {
		GEng()->render = new RenderNull(st);

	} else if (GEng()->cmdline->render_class == "ascii") {
		GEng()->render = new RenderAscii(st);

	} else {
		GEng()->render = new RenderOpenGL(st, this->gl);
	}

	// TODO: Load these settings from a config file
	GEng()->render->setScreenSize(1000, 700, this->fullscreen);
}


/**
* Init audio engine based on config options
**/
void ClientConfig::initAudio(GameState *st)
{
	if (GEng()->cmdline->audio_class == "null") {
		GEng()->audio = new AudioNull(st);

	} else {
		GEng()->audio = new AudioSDLMixer(st);
	}
}


/**
* Init physics engine based on config options
**/
void ClientConfig::initPhysics(GameState *st)
{
	new PhysicsBullet(st);
}


/**
* Init mods engine based on config options
**/
void ClientConfig::initMods(GameState *st)
{
	GEng()->mm = new ModManager();
}
