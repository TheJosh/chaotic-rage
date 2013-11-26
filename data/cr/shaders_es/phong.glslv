precision mediump float;

attribute vec3 vPosition;
attribute vec3 vNormal;
attribute vec2 vTexUV;

varying vec2 fTexUV;
varying vec3 fNormal;
varying vec3 fLightDir[2];

uniform mat4 uMVP;
uniform mat4 uMV;
uniform mat3 uN;
uniform vec3 uLightPos[2];


void main()
{
	gl_Position = uMVP * vec4(vPosition, 1.0);
	fTexUV = vTexUV;
	fNormal = uN * vNormal;
	
	vec4 pos4 = uMV * vec4(vPosition, 1.0);
	vec3 pos3 = pos4.xyz / pos4.w;
	
	for (int i = 0; i < 2; i++) {
		fLightDir[i] = normalize(uN * uLightPos[i] - pos3);
	}
}
