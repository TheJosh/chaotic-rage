// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once

class Heightmap;
class RenderOpenGL;
class GLShader;
class GLVAO;


class RendererHeightmap {
	private:
		Heightmap* heightmap;
		GLShader* shader;
		GLVAO* vao;

	public:
		/**
		* Prepare the heightmap renderer
		**/
		RendererHeightmap(RenderOpenGL* render, Heightmap* heightmap);

		/**
		* Clean up
		**/
		~RendererHeightmap();

		/**
		* Draw a heightmap
		**/
		void draw(RenderOpenGL* render);

	protected:
		/**
		* Create a shader, optimised for the specific splat rendering used
		**/
		static GLShader* createShader(RenderOpenGL* render, Heightmap* heightmap);

		/**
		* Create the 'diffuseColor' method used in splat fragment shader
		**/
		static char* createShaderFunction_diffuseColor(Heightmap* heightmap);

		/**
		* Bind the 'splat' tectures
		**/
		void bindSplatTextures();

		/**
		* Create an OpenGL mesh (array of triangle strips) from a heightmap
		* Generates a VAO
		**/
		void createVAO();
};
