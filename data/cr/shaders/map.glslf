#version 140

in vec2 fTexUV;
in vec3 fNormal;
in vec3 fLightDir;

uniform mat4 uMVP;
uniform mat4 uMV;
uniform mat4 uN;
uniform sampler2D uTex;

// Light params; these could become uniforms
const float constantAttenuation = 0.3;
const float linearAttenuation = 0.01;
const float quadraticAttenuation = 0.00001;
const vec4 diffuse = vec4(0.5, 0.5, 0.5, 1.0);
const vec4 ambient = vec4(0.1, 0.1, 0.1, 1.0);

void main()
{
	// Dot product effects strength of light
	float NdotL = max(0.0, dot(normalize(fNormal), -normalize(fLightDir)));
	
	// Calculate attenuation
	float dist = length(fLightDir);
	float att = 1.0 / (constantAttenuation + linearAttenuation*dist + quadraticAttenuation*dist*dist);
	
	// Calculate brightness
	vec4 light = (diffuse * NdotL + ambient) * att;
	
	
	gl_FragColor = texture2D(uTex, fTexUV) * light;
}
