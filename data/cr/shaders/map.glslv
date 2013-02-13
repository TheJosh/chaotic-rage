#version 140

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexUV;

out vec2 fTexUV;
out vec3 fNormal;
out vec3 fLightDir;
out vec3 fEyeVec;

uniform mat4 uMVP;
uniform mat4 uMV;
uniform mat3 uN;
uniform vec3 uLightPos;


void main()
{
	fNormal = uN * vNormal;
	
	vec3 worldPos = vec3(uMV * vec4(vPosition, 1.0f));
	fLightDir = worldPos - vec3(uMV * vec4(uLightPos, 1.0f));
	
	gl_Position = uMVP * vec4(vPosition, 1.0f);
	fTexUV = vTexUV;
}
