#version 130

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexUV;

out vec2 TexUV;
out vec4 fShadowCoord;
out float fDepth;
out vec3 csNormal;
out vec3 wsPosition;
out vec3 csEyeDirection;
out vec3 csLightDirection[10];

uniform mat4 uMVP;
uniform mat4 uM;
uniform mat4 uV;
uniform vec3 uLightPos[10];
uniform mat4 uDepthBiasMVP;


void main()
{
	gl_Position = uMVP * vec4(vPosition, 1.0f);
	
	wsPosition = (uM * vec4(vPosition, 1.0f)).xyz;
	
	vec3 csPosition = (uV * uM * vec4(vPosition, 1.0f)).xyz;
	csEyeDirection = vec3(0.0, 0.0, 0.0) - csPosition;
	
	for (int i = 0; i < 10; ++i) {
		vec3 csLightPos = (uV * vec4(uLightPos[i], 1.0f)).xyz;
		csLightDirection[i] = csLightPos + csEyeDirection;
	}
	
	csNormal = (uV * uM * vec4(vNormal, 1.0f)).xyz;
	
	TexUV = vTexUV;
	fShadowCoord = uDepthBiasMVP * vec4(vPosition, 1.0f);
	fDepth = gl_Position.z / 350f;	// TODO: Dont hard-code with far plane distance, use a uniform!
}
