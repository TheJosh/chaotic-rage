// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;


#ifndef H_SPK_GL2COLORQUADRENDERER
#define H_SPK_GL2COLORQUADRENDERER

#include "SPK_BaseQuadRenderer.h"
#include <glm/glm.hpp>


namespace SPK
{
namespace GL
{
	/**
	* Render particles using instanced quads
	*/
	class SPK_GL_PREFIX GL2ColorQuadRenderer : public BaseQuadRenderer
	{
		
		SPK_IMPLEMENT_REGISTERABLE(GL2ColorQuadRenderer)

	public :

		//////////////////
		// Constructors //
		//////////////////

		/**
		* @brief Constructor of GL2PointRenderer
		* @param size : the size of the points
		*/
		GL2ColorQuadRenderer(float size);

		/**
		* @brief Destructor of GL2PointRenderer
		*/
		virtual ~GL2ColorQuadRenderer();

		/**
		* @brief Creates and registers a new GL2PointRenderer
		* @param size : the size of the points
		* @return A new registered GL2PointRenderer
		* @since 1.04.00
		*/
		static GL2ColorQuadRenderer* create(float size);

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

	private :

		float size[2];

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


	inline GL2ColorQuadRenderer* GL2ColorQuadRenderer::create(float size)
	{
		GL2ColorQuadRenderer* obj = new GL2ColorQuadRenderer(size);
		registerObject(obj);
		return obj;
	}
}}

#endif
