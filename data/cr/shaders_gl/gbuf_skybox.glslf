#extension GL_NV_shadow_samplers_cube : enable

in vec3 fTexUV;

layout (location = 0) out vec4 gDiffuse;
layout (location = 1) out vec3 gNormal;

uniform samplerCube uTex;


void main()
{
	gDiffuse = textureCube(uTex, fTexUV);
	gNormal = vec3(0.0);
}
