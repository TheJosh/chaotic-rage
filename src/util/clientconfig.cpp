// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <string>
#include <SDL.h>
#include <confuse.h>
#include "../rage.h"
#include "../render/render_opengl_settings.h"


using namespace std;


static cfg_opt_t config_opts[] =
{
	CFG_INT((char*) "render", RENDER_OPENGL, CFGF_NONE),
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
	string filename;

	// Load config
	filename = getUserDataDir().append("client.conf");
	cfg = cfg_init(config_opts, CFGF_NONE);

	if (cfg_parse(cfg, filename.c_str()) == CFG_FILE_ERROR) {
		this->savedefault();
	}

	this->render = cfg_getint(cfg, "render");

	cfg_free(cfg);
}


/**
* Save the config
**/
void ClientConfig::save()
{
	cfg_t *cfg;
	string filename;

	// Load config
	filename = getUserDataDir().append("client.conf");
	cfg = cfg_init(config_opts, CFGF_NONE);
	cfg_parse(cfg, filename.c_str());

	// Fill from variables
	cfg_setint(cfg, "render", this->render);

	// Write to file
	FILE * fp = fopen(filename.c_str(), "w");
	if (fp != NULL) {
		cfg_print(cfg, fp);
		fclose(fp);
	}

	cfg_free(cfg);
}


void ClientConfig::savedefault()
{
	string filename;

	char * def = (char*)
		"#\n"
		"# This is the configuration file for the Chaotic Rage game client.\n"
		"#\n"
		"\n"
		"# Rendering engine:\n"
		"#   1 - OpenGL full version. Requires OpenGL 2.0 or later.\n"
		"#   2 - OpenGL compatibility version, good if you're on integrated graphics.\n"
		"render = 1\n"
		;

	filename = getUserDataDir().append("client.conf");
	FILE * fp = fopen(filename.c_str(), "w");
	if (fp != NULL) {
		fwrite(def, strlen(def), 1, fp);
		fclose(fp);
	}
}


/**
* Init rendering engine based on config options
**/
void ClientConfig::initRender(GameState *st)
{
	if (this->render == RENDER_OPENGL) {
		RenderOpenGLSettings* settings = new RenderOpenGLSettings();
		
		settings->msaa = 4;
		settings->texture_filtering = 4;
		settings->texture_anisotropy = 0;
		
		new RenderOpenGL(st, settings);

	} else if (this->render == RENDER_OPENGL_COMPAT) {
		new RenderOpenGLCompat(st);

	} else {
		exit(1);
	}

	st->render->setScreenSize(1000, 700, false);
}


/**
* Init audio engine based on config options
**/
void ClientConfig::initAudio(GameState *st)
{
	new AudioSDLMixer(st);
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
	new ModManager(st);
}
