// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "intro.h"
#include "render_opengl.h"
#include "gl_debug.h"
#include "../rage.h"
#include "../game_state.h"
#include "../game_engine.h"
#include "../render/sprite.h"
#include "../mod/mod.h"
#include "../mod/song.h"
#include "../audio/audio.h"
#include "../util/ui_update.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


using namespace std;


Intro::Intro(GameState *st)
{
	mod = new Mod(st, "data/intro");
	if (!mod) {
		reportFatalError("Intro failed to load. Is the working directory correct?");
	}

	this->st = st;
	this->render = reinterpret_cast<RenderOpenGL*>(GEng()->render);

	sg = new Song();
	sg->name = "intro";

	SDL_RWops * rw = mod->loadRWops("intro.ogg");

	#ifdef __EMSCRIPTEN__
	sg->music = Mix_LoadMUS_RW(rw);		// leaks memory
	#else
	sg->music = Mix_LoadMUS_RW(rw, 0);	// does this leak?
	#endif

	img1 = this->render->loadSprite("joshcorp.png", mod);
	img2 = this->render->loadSprite("sdl.png", mod);
	img3 = this->render->loadSprite("game.png", mod);
	text = this->render->loadSprite("loading.png", mod);

	if (img1 == NULL || img2 == NULL || img3 == NULL || text == NULL) {
		reportFatalError("Intro failed to load. Is the working directory correct?");
	}

	this->start = 0;
	this->lasttime = 0;
}


Intro::~Intro()
{
	delete(mod);
	//delete(sg);		// yep it leaks - keep the music playing even after into finished
	delete(img1);
	delete(img2);
	delete(img3);
	delete(text);
}


/**
* Prepare to run the intro
**/
void Intro::doit()
{
	GLShader *shader;

	if (sg->music != NULL) {
		GEng()->audio->playSong(sg);
	}

	start = SDL_GetTicks();
	lasttime = 0;

	CHECK_OPENGL_ERROR

	// Find 'basic' shader
	map<int, GLShader*>::const_iterator pos = render->shaders.find(SHADER_BASIC);
	if (pos == render->shaders.end()) {
		assert(false);
	} else {
		shader = pos->second;
	}

	// Set shader and uniforms
	glUseProgram(shader->p());
	glm::mat4 projection = glm::ortho<float>(
		0.0f, static_cast<float>(this->render->getWidth()),
		static_cast<float>(this->render->getHeight()), 0.0f,
		1.0f, -1.0f
	);
	glUniformMatrix4fv(shader->uniform("uMVP"), 1, GL_FALSE, &projection[0][0]);
	glUniform1i(shader->uniform("uTex"), 0);

	CHECK_OPENGL_ERROR

	this->updateUI();
}


/**
* This is called at regular intervals by the module loading code
**/
void Intro::updateUI()
{
	int time = SDL_GetTicks() - start;
	SpritePtr img;

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			exit(0);
		}
	}

	// Update the image
	if (time > 1000) {
		img = this->img3;
	} else if (time > 500) {
		img = this->img2;
	} else {
		img = this->img1;
	}

	CHECK_OPENGL_ERROR

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render img
	int x = (this->render->getWidth() - img->w) / 2;
	int y = (this->render->getHeight() - img->h) / 2;
	this->render->renderSprite(img, x, y);

	// Flashing message
	int frame = static_cast<int>(floor(time / 500.0f));
	if (frame % 2 == 0) {
		x = (this->render->getWidth() - text->w) / 2;
		y = this->render->getHeight() - text->h - 50;
		this->render->renderSprite(text, x, y);
	}

	#ifdef SDL1_VIDEO
		SDL_GL_SwapBuffers();
	#else
		SDL_GL_SwapWindow(this->render->window);
	#endif

	CHECK_OPENGL_ERROR
}
