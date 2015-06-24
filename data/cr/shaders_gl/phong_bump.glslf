#define MAX_NUM_LIGHTS 4

in vec2 TexUV0;
in vec2 TexUV1;
in vec4 fShadowCoord;
in float fDepth;
in vec3 csNormal;
in vec3 wsPosition;
in vec3 csEyeDirection;
in vec3 csLightDirection[MAX_NUM_LIGHTS];
in vec3 csTangent;
in vec3 csBitangent;
in vec3 tsEyeDirection;
in vec3 tsLightDirection[MAX_NUM_LIGHTS];

uniform mat4 uMVP;
uniform mat4 uMV;
uniform mat3 uN;
uniform sampler2D uTex;
uniform sampler2D uNormal;
uniform sampler2D uLightmap;
uniform vec3 uLightPos[MAX_NUM_LIGHTS];
uniform vec4 uLightColor[MAX_NUM_LIGHTS];
uniform vec4 uAmbient;

const float LOG2 = 1.442695;

void main()
{
	float LightPower = 25.0;
	
	vec3 tn = normalize(texture2D(uNormal, TexUV0).rgb * 2.0 - 1.0);
	vec3 te = normalize(tsEyeDirection);

	// Basic material
	vec4 matDiffuseColor = texture2D(uTex, TexUV0);
	vec4 lightmapColor = texture2D(uLightmap, TexUV1);
	vec4 matAmbientColor = uAmbient * matDiffuseColor;
	vec4 matSpecularColor = vec4(0.3, 0.3, 0.3, 1.0);

	vec4 diffuseColor = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 specularColor = vec4(0.0, 0.0, 0.0, 0.0);
	
	// Iterate lights and add color for each light
	for (int i = 0; i < MAX_NUM_LIGHTS; ++i) {
		vec3 tl = normalize(tsLightDirection[i]);

		float dist = length(uLightPos[i] - wsPosition);

		// Diffuse - bump
		float NdotL = clamp(dot(tn, tl), 0.0, 1.0);
		diffuseColor += uLightColor[i] * LightPower * uLightColor[i].a * NdotL / (dist*dist);

		// Specular - bump
		vec3 tr = reflect(-tl, tn);
		float EdotR = clamp(dot(te, tr), 0.0, 1.0);
		specularColor += uLightColor[i] * LightPower * uLightColor[i].a * pow(EdotR, 5.0) / (dist*dist);
	}

	diffuseColor = matDiffuseColor * diffuseColor + lightmapColor;
	specularColor = matSpecularColor * specularColor;

	// Fog
	float fogDensity = 1.5;
	float fogFactor = exp2(-fogDensity * fogDensity * fDepth * fDepth * LOG2);
	fogFactor = clamp(fogFactor, 0.0, 1.0);
	vec4 fogColor = vec4(0.5, 0.5, 0.6, 1.0);

	gl_FragColor = mix(fogColor, matAmbientColor + diffuseColor + specularColor, fogFactor);
}
