#version 130

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexUV;

out vec2 fTexUV;
out vec3 fNormal;
out vec3 fLightDir[2];

uniform mat4 uMVP;
uniform mat4 uMV;
uniform mat3 uN;
uniform vec3 uLightPos[2];


void main()
{
	gl_Position = uMVP * vec4(vPosition, 1.0f);
	fTexUV = vTexUV;
	fNormal = uN * vNormal;
	
	vec4 pos4 = uMV * vec4(vPosition, 1.0f);
	vec3 pos3 = pos4.xyz / pos4.w;
	
	for (int i = 0; i < 2; i++) {
		fLightDir[i] = normalize(uN * uLightPos[i] - pos3);
	}
}
