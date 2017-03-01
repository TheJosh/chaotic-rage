//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2009 - Julien Fryer - julienfryer@gmail.com				//
//																				//
// This software is provided 'as-is', without any express or implied			//
// warranty.  In no event will the authors be held liable for any damages		//
// arising from the use of this software.										//
//																				//
// Permission is granted to anyone to use this software for any purpose,		//
// including commercial applications, and to alter it and redistribute it		//
// freely, subject to the following restrictions:								//
//																				//
// 1. The origin of this software must not be misrepresented; you must not		//
//    claim that you wrote the original software. If you use this software		//
//    in a product, an acknowledgment in the product documentation would be		//
//    appreciated but is not required.											//
// 2. Altered source versions must be plainly marked as such, and must not be	//
//    misrepresented as being the original software.							//
// 3. This notice may not be removed or altered from any source distribution.	//
//////////////////////////////////////////////////////////////////////////////////


#ifndef H_SPK_GL2INSTANCEQUADRENDERER
#define H_SPK_GL2INSTANCEQUADRENDERER

#include "../spark/RenderingAPIs/OpenGL/SPK_GLRenderer.h"
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
	class SPK_GL_PREFIX GL2InstanceQuadRenderer : public GLRenderer
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
		GL2InstanceQuadRenderer();

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
		static GL2InstanceQuadRenderer* create();

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

		/**
		* Set the current View/Projection matrix
		**/
		virtual void setVP(glm::mat4 viewMatrix, glm::mat4 viewProjectionMatrix);

		/////////////
		// Options //
		/////////////

		inline void setTexture(Sprite* val)
		{
			this->texture = val;
		}

	protected :
		GLuint createShaderProgram(const char *vs, const char *fs);

	private :

		Sprite* texture;

		glm::mat4 v_matrix;
		glm::mat4 vp_matrix;

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


	inline GL2InstanceQuadRenderer* GL2InstanceQuadRenderer::create()
	{
		GL2InstanceQuadRenderer* obj = new GL2InstanceQuadRenderer();
		registerObject(obj);
		return obj;
	}
}}

#endif
