#define MAX_NUM_LAYERS 4

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec4 gDiffuse;
layout (location = 2) out vec3 gNormal;

uniform sampler2D uTex;
uniform sampler2D uAlphaMap;
uniform sampler2D uLightmap;
uniform sampler2D uLayers[MAX_NUM_LAYERS];
uniform sampler2D uDetail[MAX_NUM_LAYERS];
uniform vec3 worldSize;


/**
* Return the diffuse colour
* Code for this function is located in RendererHeightmap class
**/
vec4 diffuseColor();


void main()
{
	gPosition = FragPos / worldSize;

	gNormal = normalize(Normal);

	gDiffuse = diffuseColor();
}
