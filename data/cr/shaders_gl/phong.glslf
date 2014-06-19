#version 130

in vec2 TexUV;
in vec3 csNormal;
in vec3 csEyeDirection;
in vec3 csLightDirection;

uniform mat4 uMVP;
uniform mat4 uMV;
uniform mat4 uV;
uniform sampler2D uTex;
uniform vec4 uLightColor[2];
uniform vec4 uAmbient;

// Light params; these could become uniforms
const float constantAttenuation = 0.3;
const float linearAttenuation = 0.1;
const float quadraticAttenuation = 0.01;


void main()
{
	vec3 n = normalize(csNormal);
	vec3 l = normalize(csLightDirection);
	vec3 e = normalize(csEyeDirection);
	float dist = length(csLightDirection);

	vec4 matColor = texture2D(uTex, TexUV);

	// Diffuse
	float NdotL = clamp(dot(n, l), 0.0, 1.0);
	vec4 diffuseColor = matColor * uLightColor[0] * NdotL / (dist*dist);

	// Specular
	vec3 r = reflect(-l, n);
	float EdotR = clamp(dot(e, r), 0.0, 1.0);
	vec4 specularColor = matColor * uLightColor[0] * pow(EdotR, 5) / (dist*dist);


	//vec4 light = uAmbient;
	//float NdotL, dist, att;

	//for (int i = 0; i < 1; i++) {
		// Dot product affects strength of light
		//NdotL = clamp(dot(normalize(fNormal), normalize(fLightDir[i])), 0.0, 1.0);
	
		// Calculate attenuation
		//dist = -length(fLightDir[i]) * uLightColor[i].a;
		//att = 1.0 / (constantAttenuation + linearAttenuation*dist + quadraticAttenuation*dist*dist);
	
		// Diffuse
		//light += uLightColor[i] * NdotL;
		
		// Reflections
		//vec3 vReflection = normalize(reflect(-normalize(fLightDir[i]),normalize(fNormal)));
		//float spec = clamp(dot(normalize(fNormal), vReflection), 0.0, 1.0);
		//float fSpec = pow(spec, 64.0);
		//light += vec4(fSpec, fSpec, fSpec, 0.0f);
	//}
	
	gl_FragColor = uAmbient + diffuseColor;// + specularColor;
}
