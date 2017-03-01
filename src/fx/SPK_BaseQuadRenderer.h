// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;


#ifndef H_SPK_BASEQUADRENDERER
#define H_SPK_BASEQUADRENDERER

#include "../spark/RenderingAPIs/OpenGL/SPK_GLRenderer.h"
#include <glm/glm.hpp>


namespace SPK
{
namespace GL
{
	/**
	* Render particles using instanced quads
	*/
	class SPK_GL_PREFIX BaseQuadRenderer : public GLRenderer
	{

	public :
		//////////////////
		// Constructors //
		//////////////////

		/**
		* @brief Constructor of GL2PointRenderer
		* @param size : the size of the points
		*/
		BaseQuadRenderer();

		/**
		* @brief Destructor of GL2PointRenderer
		*/
		virtual ~BaseQuadRenderer();

		///////////////
		// Interface //
		///////////////

		virtual void render(const Group& group) = 0;

		/**
		* Call this after OpenGL init so all the buffers are created
		**/
		virtual void initGLbuffers() = 0;

		/**
		* Call before after OpenGL destruction so all the buffers are destroyed
		**/
		virtual void destroyGLbuffers() = 0;

		/**
		* Set the current View/Projection matrix
		**/
		virtual void setVP(glm::mat4 viewMatrix, glm::mat4 viewProjectionMatrix);

	protected :

		GLuint createShaderProgram(const char *vs, const char *fs);

	protected :

		glm::mat4 v_matrix;
		glm::mat4 vp_matrix;

	};
}}

#endif
