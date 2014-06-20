#version 130

in vec2 TexUV;
in vec3 csNormal;
in vec3 wsPosition;
in vec3 csEyeDirection;
in vec3 csLightDirection;

uniform mat4 uMVP;
uniform mat4 uM;
uniform mat4 uV;
uniform sampler2D uTex;
uniform vec3 uLightPos[2];
uniform vec4 uLightColor[2];
uniform vec4 uAmbient;

// Light params; these could become uniforms
const float constantAttenuation = 0.3;
const float linearAttenuation = 0.1;
const float quadraticAttenuation = 0.01;


void main()
{
	float LightPower = 10.0f;

	vec3 n = normalize(csNormal);
	vec3 l = normalize(csLightDirection);
	vec3 e = normalize(csEyeDirection);
	float dist = length(uLightPos[0] - wsPosition);

	// Basic material
	vec4 matDiffuseColor = texture2D(uTex, TexUV);
	vec4 matAmbientColor = uAmbient * matDiffuseColor;
	vec4 matSpecularColor = vec4(0.3, 0.3, 0.3, 1.0);

	// Diffuse
	float NdotL = clamp(dot(n, l), 0.0, 1.0);
	vec4 diffuseColor = matDiffuseColor * uLightColor[0] * LightPower * NdotL / (dist*dist);

	// Specular
	vec3 r = reflect(-l, n);
	float EdotR = clamp(dot(e, r), 0.0, 1.0);
	vec4 specularColor = matSpecularColor * uLightColor[0] * LightPower * pow(EdotR, 5) / (dist*dist);

	gl_FragColor = matAmbientColor + diffuseColor + specularColor;
}
