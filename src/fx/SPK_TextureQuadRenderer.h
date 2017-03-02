// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;


#ifndef H_SPK_GL2INSTANCEQUADRENDERER
#define H_SPK_GL2INSTANCEQUADRENDERER

#include "SPK_BaseQuadRenderer.h"
#include <glm/glm.hpp>

class GLVAO;
class Sprite;


namespace SPK
{
namespace GL
{
	/**
	* Render particles using instanced quads
	*/
	class SPK_GL_PREFIX GL2InstanceQuadRenderer : public BaseQuadRenderer
	{
		
		SPK_IMPLEMENT_REGISTERABLE(GL2InstanceQuadRenderer)

	public :

		//////////////////
		// Constructors //
		//////////////////

		/**
		* @brief Constructor of GL2PointRenderer
		* @param size : the size of the points
		*/
		GL2InstanceQuadRenderer(float size);

		/**
		* @brief Destructor of GL2PointRenderer
		*/
		virtual ~GL2InstanceQuadRenderer();

		/**
		* @brief Creates and registers a new GL2PointRenderer
		* @param size : the size of the points
		* @return A new registered GL2PointRenderer
		* @since 1.04.00
		*/
		static GL2InstanceQuadRenderer* create(float size);

		///////////////
		// Interface //
		///////////////

		virtual void render(const Group& group);

		/**
		* Call this after OpenGL init so all the buffers are created
		**/
		virtual void initGLbuffers();

		/**
		* Call before after OpenGL destruction so all the buffers are destroyed
		**/
		virtual void destroyGLbuffers();

		/////////////
		// Options //
		/////////////

		inline void setTexture(Sprite* val)
		{
			this->texture = val;
		}

		inline void setAtlas(int x, int y)
		{
			atlas[0] = x;
			atlas[1] = y;
		}

	private :

		Sprite* texture;
		float size[2];
		int atlas[2];

		GLuint vao;
		GLuint vboBillboardVertex;
		GLuint vboPositions;
		GLuint vboColors;
		GLuint shaderIndex;
		GLuint uniform_camUp;
		GLuint uniform_camRight;
		GLuint uniform_size;
		GLuint uniform_mvp;

		float* buffer;
		size_t buffer_sz;

	};


	inline GL2InstanceQuadRenderer* GL2InstanceQuadRenderer::create(float size)
	{
		GL2InstanceQuadRenderer* obj = new GL2InstanceQuadRenderer(size);
		registerObject(obj);
		return obj;
	}
}}

#endif
