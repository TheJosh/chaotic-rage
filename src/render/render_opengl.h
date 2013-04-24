// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include <map>

#include "../rage.h"
#include "glshader.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#define NUM_CHAR_TEX (128 - 32)

#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <guichan/opengl.hpp>
#include <guichan/opengl/openglsdlimageloader.hpp>

#include <glm/glm.hpp>

#include <assimp/scene.h>

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
	int advance;
};


class RenderOpenGL : public Render3D
{
	friend class HUD;
	friend class Menu;
	friend class Intro;
	friend class GameState;
	
	private:
		SDL_Surface * screen;
		
		// The current player being rendered (split screen)
		Player* render_player;
		
		// FreeType
		FT_Library ft;
		FT_Face face;
		vector<SpritePtr> loaded;
		FreetypeChar char_tex[NUM_CHAR_TEX];
		GLuint font_vbo;

		// Heightmap
		unsigned int ter_size;
		GLuint ter_vboid;
		GLuint ter_vaoid;
		WavefrontObj *waterobj;
		
		// Particles
		GLuint particle_vao;
		GLuint particle_vbo;

		// Debugging
		btIDebugDraw *physicsdebug;
		bool speeddebug;
		
		// Shaders
		bool shaders_loaded;		// true if loaded from a mod
		map<string, GLShader> shaders;
		
		glm::mat4 projection;	// perspective
		glm::mat4 ortho;		// ortho
		glm::mat4 view;			// camera

	public:
		unsigned int q_tex;
		unsigned int q_alias;
		unsigned int q_general;
		
	public:
		RenderOpenGL(GameState * st);
		virtual ~RenderOpenGL();
		
	protected:
		virtual SpritePtr int_loadSprite(SDL_RWops *rw, string filename);
		
	public:
		// From class Render
		virtual void setScreenSize(int width, int height, bool fullscreen);
		virtual void render();
		virtual void renderSprite(SpritePtr sprite, int x, int y);
		virtual void renderSprite(SpritePtr sprite, int x, int y, int w, int h);
		virtual void preGame();
		virtual void postGame();
		virtual void clearPixel(int x, int y);
		virtual void freeSprite(SpritePtr sprite);
		virtual int getSpriteWidth(SpritePtr sprite);
		virtual int getSpriteHeight(SpritePtr sprite);
		virtual void loadHeightmap();
		virtual void freeHeightmap();
		virtual int getWidth() { return real_width; }
		virtual int getHeight() { return real_height; }
		
		virtual void setPhysicsDebug(bool status);
		virtual bool getPhysicsDebug();
		virtual void setSpeedDebug(bool status) { this->speeddebug = status; }
		virtual bool getSpeedDebug() { return this->speeddebug; }
		
	public:
		// From class Render3D
		virtual void saveScreenshot(string filename);
		virtual void initGuichan(gcn::Gui * gui, Mod * mod);
		virtual void preVBOrender();
		virtual void postVBOrender();
		virtual void renderObj (WavefrontObj * obj);
		virtual void loadFont(string name, Mod * mod);
		virtual void renderText(string text, float x = 0.0f, float y = 0.0f, float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);
		virtual unsigned int widthText(string text);
		virtual bool loadAssimpModel(AssimpModel *am);
		
	private:
		void createWater();
		void renderAnimPlay(AnimPlay * play, Entity * e);
		void renderCharacter(char c, float &x, float &y);
		void createVBO (WavefrontObj * obj);
		void surfaceToOpenGL(SpritePtr sprite);
		void recursiveRenderAssimpModel(AssimpModel *am, AssimpNode *nd, GLShader shader, glm::mat4 transform);
		
		void mainViewport(int s, int of);
		void mainRot();
		void terrain();
		void entities();
		void physics();
		void water();
		void particles();
		void guichan();
		void hud(HUD * hud);
		void fps();
		
		void loadShaders();
		GLuint createShader(const char* code, GLenum type);
		GLShader createProgram(const char* vertex, const char* fragment, string name);
		GLShader loadProgram(Mod* mod, string name);
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


