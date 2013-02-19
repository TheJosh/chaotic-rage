#version 140

in vec2 fTexUV;
in vec3 fNormal;
in vec3 fLightDir[2];

uniform mat4 uMVP;
uniform mat4 uMV;
uniform mat4 uN;
uniform sampler2D uTex;
uniform vec3 uLightColor[2];

// Light params; these could become uniforms
const float constantAttenuation = 0.3;
const float linearAttenuation = 0.01;
const float quadraticAttenuation = 0.00001;
const vec4 ambient = vec4(0.1, 0.1, 0.1, 1.0);

void main()
{
	vec4 light = ambient;
	float NdotL, dist, att;

	for (int i = 0; i < 2; i++) {
		// Dot product affects strength of light
		NdotL = max(0.0, dot(normalize(fNormal), -normalize(fLightDir[i])));
	
		// Calculate attenuation
		dist = length(fLightDir[i]);
		att = 1.0 / (constantAttenuation + linearAttenuation*dist + quadraticAttenuation*dist*dist);
	
		// Calculate brightness
		light += vec4(uLightColor[i], 1.0f) * NdotL * att;
	}
	
	gl_FragColor = texture2D(uTex, fTexUV) * light;
}
