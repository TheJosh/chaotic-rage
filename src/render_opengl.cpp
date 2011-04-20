// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <algorithm>
#include <GL/glew.h>
#include <GL/gl.h>
#if defined(__WIN32__)
	#include <GL/glext.h>
#endif
#include <GL/glu.h>
#include <SDL_image.h>
#include <math.h>
#include "rage.h"

using namespace std;



//static bool ZIndexPredicate(const Entity * e1, const Entity * e2)
//{
//	return e1->z < e2->z;
//}


RenderOpenGL::RenderOpenGL(GameState * st) : Render(st)
{
	this->screen = NULL;
	this->x = 1;
	this->y = 1;
	this->z = -4;
}

RenderOpenGL::~RenderOpenGL()
{
}


/**
* Sets the screen size
**/
void RenderOpenGL::setScreenSize(int width, int height, bool fullscreen)
{
	int flags;
	
	// TODO: we need to keep track of all textures
	// because when this method runs again, we need to re-load everything
	// back into OpenGL
	// Priority: Medium
	
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	
	flags = SDL_OPENGL;
	if (fullscreen) flags |= SDL_FULLSCREEN;
	
	this->screen = SDL_SetVideoMode(width, height, 32, flags);
	if (screen == NULL) {
		fprintf(stderr, "Unable to set %ix%i video: %s\n", width, height, SDL_GetError());
		exit(1);
	}
	
	if (atof((char*) glGetString(GL_VERSION)) < 2.0) {
		fprintf(stderr, "OpenGL 2.0 or later is required, but not supported on this system.");
		exit(1);
	}
	
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "Glew Error: %s\n", glewGetErrorString(err));
		exit(1);
	}
	
	// The 'virtual' size is 1000px high, with the proper width for your monitor.
	this->virt_height = 1000;
	this->virt_width = (int) floor(this->virt_height * (float)width / (float)height);
	
	SDL_WM_SetCaption("Chaotic Rage", "Chaotic Rage");
	SDL_ShowCursor(SDL_DISABLE);
	
	flags = IMG_INIT_PNG;
	int initted = IMG_Init(flags);
	if ((initted & flags) != flags) {
		fprintf(stderr, "Failed to init required png support!\n");
		exit(1);
	}
	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	
	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	glViewport(0, 0, width, height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluPerspective(45.0f, 1.0f, 1.0f, 1500.f);
	glScalef (1.0f, -1.0f, 1.0f);
	glTranslatef(-500, -500, -1250.0f);
	
	//glOrtho(0.0f, this->virt_width, this->virt_height, 0.0f, -1.0f, 1.0f);
	//glFrustum(0.0f, this->virt_width, this->virt_height, 0.0f, 1, 500);
	//glTranslatef(0, 0, -490.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


/**
* Internal sprite loading from a SDL_RWops
**/
SpritePtr RenderOpenGL::int_loadSprite(SDL_RWops *rw, string filename)
{
	GLenum texture_format;
	GLint num_colors;
	SDL_Surface * surf;
	
	surf = IMG_Load_RW(rw, 0);
	if (surf == NULL) {
		fprintf(stderr, "Couldn't load sprite '%s'.\n", filename.c_str());
		load_err = true;
		return NULL;
	}
	
	if ((surf->w & (surf->w - 1)) != 0) {
		DEBUG("Bitmap '%s' width is not a power of 2.\n", filename.c_str());
	}
	
	if ((surf->h & (surf->h - 1)) != 0) {
		DEBUG("Bitmap '%s' height is not a power of 2.\n", filename.c_str());
	}
	
	// Determine OpenGL import type
	num_colors = surf->format->BytesPerPixel;
	if (num_colors == 4) {
		if (surf->format->Rmask == 0x000000ff) {
			texture_format = GL_RGBA;
		} else {
			texture_format = GL_BGRA;
		}
		
	} else if (num_colors == 3) {
		if (surf->format->Rmask == 0x000000ff) {
			texture_format = GL_RGB;
		} else {
			texture_format = GL_BGR;
		}
		
	} else {
		fprintf(stderr, "Bitmap '%s' not in 32-bit or 24-bit colour; unable to load into OpenGL\n", filename.c_str());
		load_err = true;
		return NULL;
	}
	
	// Create the sprite object
	SpritePtr sprite = new struct sprite();
	sprite->w = surf->w;
	sprite->h = surf->h;
	sprite->orig = surf;
	
	// Open texture handle
	glGenTextures(1, &sprite->pixels);
	glBindTexture(GL_TEXTURE_2D, sprite->pixels);
	
	// Set stretching properties
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surf->w, surf->h, 0, texture_format, GL_UNSIGNED_BYTE, surf->pixels);
	
	//SDL_FreeSurface(surf);
	
	return sprite;
}


/**
* Renders a sprite.
* Should only be used if the the caller was called by this classes 'Render' function.
**/
void RenderOpenGL::renderSprite(SpritePtr sprite, int x, int y)
{
	renderSprite(sprite, x, y, sprite->w, sprite->h);
}

/**
* Renders a sprite.
* Should only be used if the the caller was called by this classes 'Render' function.
**/
void RenderOpenGL::renderSprite(SpritePtr sprite, int x, int y, int w, int h)
{
	glBindTexture(GL_TEXTURE_2D, sprite->pixels);
 	
	glBegin(GL_QUADS);
		// Bottom-left vertex (corner)
		glTexCoord2i( 0, 1 );
		glVertex2i( x, y + w );
		
		// Bottom-right vertex (corner)
		glTexCoord2i( 1, 1 );
		glVertex2i( x + w, y + h );
		
		// Top-right vertex (corner)
		glTexCoord2i( 1, 0 );
		glVertex2i( x + w, y );
		
		// Top-left vertex (corner)
		glTexCoord2i( 0, 0 );
		glVertex2i( x, y );
	glEnd();
}


void RenderOpenGL::preGame()
{
	AnimModel *model = st->getMod(0)->getAnimModel("cube");
	this->test = new AnimPlay(model);
}

void RenderOpenGL::postGame()
{
	delete(this->test);
}


/**
* Clears all colour from a given pixel for a given sprite
**/
void RenderOpenGL::clearPixel(int x, int y)
{
	// TODO: code this
	// Priority: Low
	//setPixel(walls->surf, x, y, this->colourkey);
}

/**
* Free sprite memory
**/
void RenderOpenGL::freeSprite(SpritePtr sprite)
{
	glDeleteTextures(1, &sprite->pixels);
	delete(sprite);
}

/**
* Returns sprite width
**/
int RenderOpenGL::getSpriteWidth(SpritePtr sprite)
{
	return sprite->w;
}

/**
* Returns sprite height
**/
int RenderOpenGL::getSpriteHeight(SpritePtr sprite)
{
	return sprite->h;
}


#define BUFFER_OFFSET(i) ((char *)NULL + (i))


/**
* Builds a VBO for this object
**/
void RenderOpenGL::createVBO (WavefrontObj * obj)
{
	GLuint vboid;
	GLuint iboid;
	
	glGenBuffers(1, &vboid);
	glBindBuffer(GL_ARRAY_BUFFER, vboid);
	
	VBOvertex vertexes[obj->faces.size() * 3];
	Uint16 index[obj->faces.size() * 3];
	
	int j = 0;
	for (unsigned int i = 0; i < obj->faces.size(); i++) {
		Face * f = &obj->faces.at(i);
		
		Vertex * v;
		Vertex * vn;
		TexUV * t;
		
		v = &obj->vertexes.at(f->v1 - 1);
		vn = &obj->normals.at(f->n1 - 1);
		t = &obj->texuvs.at(f->t1 - 1);
		
		vertexes[j].x = v->x; vertexes[j].y = v->y; vertexes[j].z = v->z;
		vertexes[j].nx = vn->x; vertexes[j].ny = vn->y; vertexes[j].nz = vn->z;
		vertexes[j].tx = t->x; vertexes[j].ty = 1.0 - t->y;
		index[j] = j;
		j++;
		
		
		v = &obj->vertexes.at(f->v2 - 1);
		vn = &obj->normals.at(f->n2 - 1);
		t = &obj->texuvs.at(f->t2 - 1);
		
		vertexes[j].x = v->x; vertexes[j].y = v->y; vertexes[j].z = v->z;
		vertexes[j].nx = vn->x; vertexes[j].ny = vn->y; vertexes[j].nz = vn->z;
		vertexes[j].tx = t->x; vertexes[j].ty = 1.0 - t->y;
		index[j] = j;
		j++;
		
		
		v = &obj->vertexes.at(f->v3 - 1);
		vn = &obj->normals.at(f->n3 - 1);
		t = &obj->texuvs.at(f->t3 - 1);
		
		vertexes[j].x = v->x; vertexes[j].y = v->y; vertexes[j].z = v->z;
		vertexes[j].nx = vn->x; vertexes[j].ny = vn->y; vertexes[j].nz = vn->z;
		vertexes[j].tx = t->x; vertexes[j].ty = 1.0 - t->y;
		index[j] = j;
		j++;
		
	}
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexes), &vertexes[0].x, GL_STATIC_DRAW);
	
	
	// TODO:
	// We don't need an index bugger of index[j] == j
	// just use glDrawArrays
	// (thanks ##OpenGL)
	glGenBuffers(1, &iboid);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), &index, GL_STATIC_DRAW);
	
	obj->ibo_count = sizeof(index) / sizeof(index[0]);
	obj->vbo = vboid;
	obj->ibo = iboid;
}


/**
* Renders an object
**/
void RenderOpenGL::renderObj (WavefrontObj * obj)
{
	if (obj->ibo_count == 0) this->createVBO(obj);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glBindBuffer(GL_ARRAY_BUFFER, obj->vbo);
	glVertexPointer(3, GL_FLOAT, 32, BUFFER_OFFSET(0));
	glNormalPointer(GL_FLOAT, 32, BUFFER_OFFSET(12));
	glClientActiveTexture(GL_TEXTURE0);
	glTexCoordPointer(2, GL_FLOAT, 32, BUFFER_OFFSET(24));
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->ibo);
	glDrawElements(GL_TRIANGLES, obj->ibo_count, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


void RenderOpenGL::renderAnimPlay(AnimPlay * play)
{
	AnimModel * model;
	
	model = play->getModel();
	if (model == NULL) return;
	
	int frame = play->getFrame();
	
	for (unsigned int d = 0; d < model->meshframes.size(); d++) {
		if (model->meshframes[d]->frame != frame) continue;
		if (model->meshframes[d]->mesh == NULL) continue;
		if (model->meshframes[d]->texture == NULL) continue;
		
		glBindTexture(GL_TEXTURE_2D, model->meshframes[d]->texture->pixels);
		
		glPushMatrix();
		
		glTranslatef(model->meshframes[d]->px, model->meshframes[d]->py, model->meshframes[d]->pz);
		glRotatef(model->meshframes[d]->rx, 1, 0, 0);
		glRotatef(model->meshframes[d]->ry, 0, 1, 0);
		glRotatef(model->meshframes[d]->rz, 0, 0, 1);
		glScalef(model->meshframes[d]->sx, model->meshframes[d]->sy, model->meshframes[d]->sz);
		
		this->renderObj(model->meshframes[d]->mesh);
		
		glPopMatrix();
	}
}

/**
* Renders
**/
void RenderOpenGL::render()
{
	unsigned int i, j;
	AnimPlay * play;
	
	int x, y;	// for general use
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	
	
	
	// Background
	/*glLoadIdentity();
	glTranslatef(this->virt_width / 2, this->virt_height / 2, 0);
	glRotatef(st->curr_player->angle, 0, 0, 1);
	glTranslatef(0 - w / 2, 0 - h / 2, 0);
	this->renderSprite(st->map->background, 0, 0, w, h);*/
	
	
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// Main map rotation
	glLoadIdentity();
	glTranslatef(this->virt_width / 2, this->virt_height / 2, 0);
	
	if (st->curr_player == NULL) {
		glRotatef(22, 0, 0, 1);
		glRotatef(12, 1, 0, 0);
		glTranslatef(0 - st->curr_map->width / 2, 0 - st->curr_map->height / 2, 0);
		
	} else {
		glRotatef(st->curr_player->angle, 0, 0, 1);
		glTranslatef(0 - st->curr_player->x, 0 - st->curr_player->y, 600);
	}
	
	
	// Lights
	glPushMatrix();
		glTranslatef(1000, 1000, 100);
		
		GLfloat ambientLight[] = { 0.4f, 0.4f, 0.4f, 1.0f };
		GLfloat diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		GLfloat position[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
		glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		glEnable(GL_LIGHT0);
	glPopMatrix();
	

	// Render map
	for (i = 0; i < st->curr_map->areas.size(); i++) {
		Area * a = st->curr_map->areas[i];
		
		glTranslatef(0, 0, 1);
		
		glPushMatrix();
		
		x = a->x + a->width / 2;
		y = a->y + a->height / 2;
		
		glTranslatef(x, y, 0);
		glRotatef(0 - a->angle, 0, 0, 1);
		glTranslatef(0 - x, 0 - y, 0);
		
		glBindTexture(GL_TEXTURE_2D, a->type->surf->pixels);
 		
 		a->width = a->height = 50;
 		
 		float texw = 1.0;
 		float texh = 1.0;
 		if (! a->type->stretch) {
 			texw = ((float)a->width) / ((float)a->type->surf->w);
 			texh = ((float)a->height) / ((float)a->type->surf->h);
 		}
 		
 		
 		glNormal3f(0, 0, 1);
 		
		glBegin(GL_QUADS);
			for (int x = 0; x < 2000; x += a->width) {
			for (int y = 0; y < 2000; y += a->width) {
				
				// Bottom-left vertex (corner)
				glTexCoord2f( 0.0, texh );
				glVertex3i( x, y + a->height, 0 );
				
				// Bottom-right vertex (corner)
				glTexCoord2f( texw, texh );
				glVertex3i( x + a->width, y + a->height, 0 );
				
				// Top-right vertex (corner)
				glTexCoord2f( texw, 0.0 );
				glVertex3i( x + a->width, y, 0 );
				
				// Top-left vertex (corner)
				glTexCoord2f( 0.0, 0.0 );
				glVertex3i( x, y, 0 );
				
			}
			}
		glEnd();
		
		glPopMatrix();
	}
	
	glTranslatef(0, 0, 10);
	
	// Entities
	//std::sort(st->entities.begin(), st->entities.end(), ZIndexPredicate);
	for (i = 0; i < st->entities.size(); i++) {
		Entity *e = st->entities.at(i);
		
		
		glPushMatrix();
		
		glTranslatef(e->x, e->y, e->z);
		glRotatef(0 - e->angle, 0, 0, 1);
		
		
		AnimPlay * list [SPRITE_LIST_LEN] = {NULL, NULL, NULL, NULL};
		e->getAnimModel(list);
		
		for (j = 0; j < SPRITE_LIST_LEN; j++) {
			play = list[j];
			if (play == NULL) break;
			
			renderAnimPlay(play);
		}
		
		
		glPopMatrix();
	}
	
	// Testing: show collide boxes
	for (list<CollideBox*>::iterator it = st->collideboxes.begin(); it != st->collideboxes.end(); it++) {
		CollideBox *c = (*it);
		
		glPushMatrix();
		
		glTranslatef(c->x, c->y, 100);
		glScalef(c->radius / 50.0, c->radius / 50.0, c->radius / 50.0);
		renderAnimPlay(this->test);
		
		glPopMatrix();
	}
	
	
	// HUD
	glLoadIdentity();
	glTranslatef(0, 0, 40);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	st->hud->render(this);
	
	SDL_GL_SwapBuffers();
}


