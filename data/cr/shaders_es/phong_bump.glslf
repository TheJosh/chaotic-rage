precision mediump float;

varying vec2 fTexUV;
varying vec3 fLightDir[2];

uniform mat4 uMVP;
uniform mat4 uMV;
uniform mat4 uN;
uniform sampler2D uTex;
uniform sampler2D uNormals;
uniform vec4 uLightColor[2];
uniform vec4 uAmbient;

// Light params; these could become uniforms
const float constantAttenuation = 0.3f;
const float linearAttenuation = 0.1f;
const float quadraticAttenuation = 0.01f;


void main()
{
	vec4 light = uAmbient;
	
	vec3 normal = normalize(2.0f * texture2D(uNormals, fTexUV).rgb - 1.0);
	
	for (int i = 0; i < 2; i++) {
		float NdotL, dist, att;
		
		// Dot product affects strength of light
		NdotL = max(0.0f, dot(fLightDir[i], normal));
	
		// Calculate attenuation
		//dist = length(fLightDir[i]) * uLightColor[i].a;
		//att = 1.0f / (constantAttenuation + linearAttenuation*dist + quadraticAttenuation*dist*dist);
	
		// Calculate brightness
		light += uLightColor[i] * NdotL;//; * att;
	}
	
	gl_FragColor = texture2D(uTex, fTexUV) * light;
}
