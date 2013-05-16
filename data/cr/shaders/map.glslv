#version 140

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
	
	vec4 worldPos = uMV * vec4(vPosition, 1.0f);

	for (int i = 0; i < 2; i++) {
		vec4 dir = worldPos - uMV * vec4(uLightPos[i], 1.0f);
		fLightDir[i] = dir.xyz;
	}
}
