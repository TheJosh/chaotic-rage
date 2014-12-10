precision mediump float;

#define MAX_NUM_LIGHTS 4

attribute vec3 vPosition;
attribute vec3 vNormal;
attribute vec2 vTexUV;

varying vec2 TexUV;
varying vec4 fShadowCoord;
varying float fDepth;
varying vec3 csNormal;
varying vec3 wsPosition;
varying vec3 csEyeDirection;
varying vec3 csLightDirection[MAX_NUM_LIGHTS];

uniform mat4 uMVP;
uniform mat4 uM;
uniform mat3 uMN;
uniform mat4 uV;
uniform vec3 uLightPos[MAX_NUM_LIGHTS];
uniform vec4 uLightColor[MAX_NUM_LIGHTS];
uniform mat4 uDepthBiasMVP;


void main()
{	gl_Position = uMVP * vec4(vPosition, 1.0);
	
	wsPosition = (uM * vec4(vPosition, 1.0)).xyz;
	
	csEyeDirection = vec3(0.0, 0.0, 0.0) - (uV * uM * vec4(vPosition, 1.0)).xyz;
	
	for (int i = 0; i < MAX_NUM_LIGHTS; ++i) {
		vec3 csLightPos = (uV * vec4(uLightPos[i], 1.0)).xyz;
		csLightDirection[i] = csLightPos + csEyeDirection;
	}
	
	csNormal = (mat3(uV) * uMN * vNormal).xyz;
	
	TexUV = vTexUV;
	fShadowCoord = uDepthBiasMVP * vec4(vPosition, 1.0);
	fDepth = gl_Position.z / 350.0;	// TODO: Dont hard-code with far plane distance, use a uniform!
}
