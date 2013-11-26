#pragma once

#include <vector>
#include <btBulletDynamicsCommon.h>

using namespace std;


class GLVAO;

class Vertex {
	public:
		float x, y, z;
		btVector3 toBtVector3() { return btVector3(x, y, z); }
};

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
		
		GLVAO* vao;
		unsigned int count;
		
		Vertex size;
		
	public:
		WavefrontObj();
		void calcBoundingSize();
};
