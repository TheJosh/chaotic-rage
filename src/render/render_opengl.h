// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "../rage.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#define NUM_CHAR_TEX (128 - 32)

#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <guichan/opengl.hpp>
#include <guichan/opengl/openglsdlimageloader.hpp>

#include "LinearMath/btIDebugDraw.h"



struct VBOvertex
{
	float x, y, z;        // Vertex
	float nx, ny, nz;     // Normal
	float tx, ty;         // Tex
};

struct FreetypeChar
{
	GLuint tex;
	int x, y, w, h;
	float tx, ty;
};


class RenderOpenGL : public Render
{
	friend class HUD;
	friend class Menu;
	friend class Intro;
	friend class GameState;
	
	private:
		SDL_Surface * screen;
		
		// Size of the actual screen
		int real_width;
		int real_height;
		
		// Size of the 'virtual' screen.
		int virt_width;
		int virt_height;
		
		// Width and height of the desktop at game begin
		int desktop_width;
		int desktop_height;
		
		// The current player being rendered (split screen)
		Unit* render_player;
		
		// FreeType
		FT_Library ft;
		FT_Face face;
		
		// Caches
		vector<SpritePtr> loaded;
		FreetypeChar char_tex[NUM_CHAR_TEX];
		
		// Heightmap
		unsigned int ter_size;
		GLuint ter_vboid;
		
		// Debugging
		AnimPlay *test;
		btIDebugDraw *physicsdebug;
		
		
	public:
		int viewmode;
		int tx, ty, tz;		// these are for debugging I think
		int rx, ry, rz;		// these too
		
		
	protected:
		virtual SpritePtr int_loadSprite(SDL_RWops *rw, string filename);
		
	public:
		virtual void setScreenSize(int width, int height, bool fullscreen, int multisample);
		virtual void render();
		virtual void renderSprite(SpritePtr sprite, int x, int y);
		virtual void renderSprite(SpritePtr sprite, int x, int y, int w, int h);
		virtual void preGame();
		virtual void postGame();
		virtual void clearPixel(int x, int y);
		virtual void freeSprite(SpritePtr sprite);
		virtual int getSpriteWidth(SpritePtr sprite);
		virtual int getSpriteHeight(SpritePtr sprite);
		virtual void enablePhysicsDebug();
		virtual void disablePhysicsDebug();
		virtual void loadHeightmap(SpritePtr sprite);
		virtual void freeHeightmap(SpritePtr sprite);
		
	public:
		RenderOpenGL(GameState * st);
		virtual ~RenderOpenGL();
		
		void saveScreenshot(string filename);
		void initGuichan(gcn::Gui * gui, Mod * mod);
		
	protected:
		void renderObj (WavefrontObj * obj);
		void renderAnimPlay(AnimPlay * play);
		void renderText(string text, float x = 0.0, float y = 0.0, float r = 1.0, float g = 1.0, float b = 1.0);
		void renderCharacter(char c);
		void loadFont(string name, Mod * mod);
		void deadRot();
		
	private:
		void createVBO (WavefrontObj * obj);
		void surfaceToOpenGL(SpritePtr sprite);
		
		void mainViewport(int s, int of);
		void background();
		void mainRot();
		void lights();
		void map();
		void entities();
		void physics();
		void particles();
		void guichan();
		void hud();
};



/**
* OpenGL debug drawing class.
*
* From bullet/demos/opengl
**/
class GLDebugDrawer : public btIDebugDraw
{
	private:
		int m_debugMode;
		
	public:
		GLDebugDrawer();
		
		virtual void	drawLine(const btVector3& from,const btVector3& to,const btVector3& fromColor, const btVector3& toColor);
		
		virtual void	drawLine(const btVector3& from,const btVector3& to,const btVector3& color);
		
		virtual void	drawSphere (const btVector3& p, btScalar radius, const btVector3& color);
		
		virtual void	drawBox (const btVector3& boxMin, const btVector3& boxMax, const btVector3& color, btScalar alpha);
		
		virtual void	drawTriangle(const btVector3& a,const btVector3& b,const btVector3& c,const btVector3& color,btScalar alpha);
		
		virtual void	drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color);
		
		virtual void	reportErrorWarning(const char* warningString);
		
		virtual void	draw3dText(const btVector3& location,const char* textString);
		
		virtual void	setDebugMode(int debugMode);
		
		virtual int		getDebugMode() const { return m_debugMode;}
};


