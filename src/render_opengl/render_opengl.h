// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <SDL.h>
#include <map>

#include "../rage.h"
#include "glshader.h"
#include "../render/render_3d.h"

#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <guichan/opengl.hpp>
#include <guichan/opengl/openglsdlimageloader.hpp>

#include <glm/glm.hpp>

#include <assimp/scene.h>



struct VBOvertex
{
	float x, y, z;        // Vertex
	float nx, ny, nz;     // Normal
	float tx, ty;         // Tex
};


class GLVAO;
class RenderOpenGLSettings;
class btIDebugDraw;
class OpenGLFont;
class Heightmap;
class RendererHeightmap;


// temporary class!
class PlayEntity {
	public:
		AnimPlay* play;
		Entity* e;
};


class RenderOpenGL : public Render3D
{
	friend class HUD;
	friend class Menu;
	friend class Intro;
	friend class GameState;
	friend class OpenGLFont;
	friend class RendererHeightmap;

	private:
		#ifdef SDL1_VIDEO
			SDL_Surface *window;
		#else
			SDL_Window *window;
			SDL_GLContext glcontext;
		#endif

		// The current player being rendered (split screen)
		Player* render_player;
		glm::vec3 render_player_pos;

		// Loaded sprites
		vector<SpritePtr> loaded;

		OpenGLFont* font;
		OpenGLFont* gui_font;

		// VBOs
		GLuint sprite_vbo;

		// VAOs
		GLVAO* ter_vao;
		GLVAO* skybox_vao;

		// Shadowing
		GLuint shadow_framebuffer;
		GLuint shadow_depth_tex;

		// Heightmap
		unsigned int ter_size;

		// Water
		WavefrontObj *waterobj;

		// Debugging
		btIDebugDraw *physicsdebug;
		bool speeddebug;

		// Shaders
		bool shaders_loaded;		// true if loaded from a mod
		bool shaders_error;			// true if a shader load failed
		map<int, GLShader*> shaders;	// TODO: switch to a vector?

		glm::mat4 projection;	// perspective
		glm::mat4 ortho;		// ortho
		glm::mat4 ortho_guichan;// ortho for guichan
		glm::mat4 view;			// camera
		glm::mat4 depthmvp;		// light
		glm::vec3 camera;

		RenderOpenGLSettings* settings;
		int min_filter;
		int mag_filter;

		SPK::GL::GLRenderer* particle_renderer;

		static const int SHADOW_MAP_WIDTH = 2048;
		static const int SHADOW_MAP_HEIGHT = 2048;


		// This is the INITIAL version of the mirrored entity list
		// Eventually we will rejig this list to be further optimised
		// to match the main the render path
		vector<PlayEntity> animations;

		// Lights
		vector<Light*> lights;
		bool torch;
		glm::vec4 ambient;

		SpritePtr ambient_daynight;

	public:
		RenderOpenGL(GameState * st, RenderOpenGLSettings* settings);
		virtual ~RenderOpenGL();

	protected:
		virtual SpritePtr int_loadSprite(SDL_RWops *rw, string filename);

	public:
		// From class Render
		virtual void setScreenSize(int width, int height, bool fullscreen);
		virtual void setMouseGrab(bool newval);
		virtual void render();
		virtual void renderSprite(SpritePtr sprite, int x, int y);
		virtual void renderSprite(SpritePtr sprite, int x, int y, int w, int h);
		void renderSprite(GLuint texture, int x, int y, int w, int h);
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
		virtual void addAnimPlay(AnimPlay* play, Entity* e);
		virtual void remAnimPlay(AnimPlay* play);
		virtual void addLight(Light* light);
		virtual void remLight(Light* light);
		virtual void setTorch(bool on);
		virtual void setAmbient(glm::vec4 ambient);
		virtual void loadCommonData();

		virtual void setPhysicsDebug(bool status);
		virtual bool getPhysicsDebug();
		virtual void setSpeedDebug(bool status) { this->speeddebug = status; }
		virtual bool getSpeedDebug() { return this->speeddebug; }

	public:
		// From class Render3D
		virtual void saveScreenshot(string filename);
		virtual void initGuichan(gcn::Gui * gui, Mod * mod);
		virtual void loadFont(string name, Mod * mod);
		virtual void renderText(string text, int x = 0, int y = 0, float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);
		virtual unsigned int widthText(string text);
		virtual SpritePtr loadCubemap(string filename_base, string filename_ext, Mod * mod);
		virtual SpritePtr load1D(string filename, Mod* mod);
		virtual void mouseRaycast(int x, int y, btVector3& start, btVector3& end);

		void setSettings(RenderOpenGLSettings* settings);
		RenderOpenGLSettings* getSettings();

		bool reloadShaders();

	private:
		void createWater();
		void createSkybox();
		
		// Buffers needed for shadowing
		void createShadowBuffers();

		// Render to those buffers so it all works
		void entitiesShadowMap();

		void renderCharacter(Uint32 c, float &x, float &y);
		void createVBO (WavefrontObj * obj);
		void surfaceToOpenGL(SpritePtr sprite);

		// Entity rendering
		GLShader* determineAssimpModelShader(AssimpModel* play);
		void renderAnimPlay(AnimPlay* play, Entity * e);
		void renderAnimPlay(AnimPlay* play, const glm::mat4 &modelMatrix);
		void recursiveRenderAssimpModelStatic(AnimPlay* ap, AssimpModel *am, AssimpNode *nd, GLShader *shader, const glm::mat4 &modelMatrix);
		void recursiveRenderAssimpModelBones(AnimPlay* ap, AssimpModel *am, AssimpNode *nd, GLShader *shader);

		void mainViewport(int s, int of);
		void mainRot();
		void skybox();
		void terrain();
		void entities();
		void physics();
		void water();
		void particles();
		void guichan();
		void hud(HUD * hud);
		void fps();

		void loadShaders();
		GLShader* createProgram(const char* vertex, const char* fragment);
		GLShader* loadProgram(Mod* mod, string name);
		GLShader* loadProgram(Mod* mod, string vertex_name, string fragment_name);
		void deleteProgram(GLShader* shader);
		void setupShaders();
};
