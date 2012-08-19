#pragma once

#include <vector>

using namespace std;


typedef struct vertex {
	float x, y, z;
} Vertex;

typedef struct texuv {
	float x, y;
} TexUV;

typedef struct face {
	int v1,v2,v3;
	int t1,t2,t3;
	int n1,n2,n3;
	int mat;
} Face;

class WavefrontObj {
	public:
		vector<Vertex> vertexes;
		vector<TexUV> texuvs;
		vector<Vertex> normals;
		vector<string> materials;
		vector<Face> faces;
		
		unsigned int ibo_count;
		unsigned int vbo;		// vertex, normal, textures
		unsigned int ibo;		// vertex index
		
		Vertex size;
		
	public:
		void calcBoundingSize();
};


WavefrontObj * loadObj(string filename);
