#version 140

in vec2 fTexUV;
in vec3 fNormal;
in vec3 fLightDir[2];

uniform mat4 uMVP;
uniform mat4 uMV;
uniform mat4 uN;
uniform sampler2D uTex;
uniform vec4 uLightColor[2];
uniform vec4 uAmbient;

// Light params; these could become uniforms
const float constantAttenuation = 0.3;
const float linearAttenuation = 0.1;
const float quadraticAttenuation = 0.01;

void main()
{
	vec4 light = uAmbient;
	float NdotL, dist, att;

	for (int i = 0; i < 2; i++) {
		// Dot product affects strength of light
		NdotL = max(0.0, dot(normalize(fNormal), -normalize(fLightDir[i])));
	
		// Calculate attenuation
		dist = length(fLightDir[i]) * uLightColor[i].a;
		att = 1.0 / (constantAttenuation + linearAttenuation*dist + quadraticAttenuation*dist*dist);
	
		// Calculate brightness
		light += uLightColor[i] * NdotL * att;
	}
	
	gl_FragColor = texture2D(uTex, fTexUV) * light;
}
