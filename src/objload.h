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

typedef struct wavefrontobj {
	vector<Vertex> vertexes;
	vector<TexUV> texuvs;
	vector<Vertex> normals;
	vector<string> materials;
	vector<Face> faces;
} WavefrontObj;


WavefrontObj * loadObj(string filename);
