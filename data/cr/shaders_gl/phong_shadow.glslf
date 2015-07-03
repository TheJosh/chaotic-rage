#define MAX_NUM_LIGHTS 4

in vec2 TexUV0;
in vec4 fShadowCoord;
in float fDepth;
in vec3 csNormal;
in vec3 wsPosition;
in vec3 csEyeDirection;
in vec3 csLightDirection[MAX_NUM_LIGHTS];

uniform mat4 uMVP;
uniform mat4 uMV;
uniform mat3 uN;
uniform sampler2D uTex;
uniform sampler2D uShadowMap;
uniform sampler1D uDayNight;
uniform vec3 uLightPos[MAX_NUM_LIGHTS];
uniform vec4 uLightColor[MAX_NUM_LIGHTS];
uniform vec4 uAmbient;
uniform float uTimeOfDay;

const float LOG2 = 1.442695;

void main()
{
	float LightPower = 25.0;
	
	vec3 n = normalize(csNormal);
	vec3 e = normalize(csEyeDirection);

	// Basic material
	vec4 todColor = texture(uDayNight, uTimeOfDay);
	vec4 matDiffuseColor = texture2D(uTex, TexUV0);
	vec4 matAmbientColor = todColor * matDiffuseColor;
	vec4 matSpecularColor = vec4(0.3, 0.3, 0.3, 1.0);

	vec4 diffuseColor = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 specularColor = vec4(0.0, 0.0, 0.0, 0.0);
	
	// Iterate lights and add color for each light
	for (int i = 0; i < MAX_NUM_LIGHTS; ++i) {
		vec3 l = normalize(csLightDirection[i]);
		float dist = length(uLightPos[i] - wsPosition);
		
		// Diffuse
		float NdotL = clamp(dot(n, l), 0.0, 1.0);
		diffuseColor += uLightColor[i] * LightPower * uLightColor[i].a * NdotL / (dist*dist);

		// Specular
		vec3 r = reflect(-l, n);
		float EdotR = clamp(dot(e, r), 0.0, 1.0);
		specularColor += uLightColor[i] * LightPower * uLightColor[i].a * pow(EdotR, 5.0) / (dist*dist);
	}
	
	diffuseColor = matDiffuseColor * diffuseColor;
	specularColor = matSpecularColor * specularColor;
	
	
	// Shadow
	float shadowBias = 0.0005;
	float visibility = 1.0;
	if (texture2D(uShadowMap, fShadowCoord.xy).r < fShadowCoord.z - shadowBias) {
		visibility = 0.5;
	}

	// Fog
	float fogDensity = 1.5;
	float fogFactor = exp2(-fogDensity * fogDensity * fDepth * fDepth * LOG2);
	fogFactor = clamp(fogFactor, 0.0, 1.0);
	vec4 fogColor = todColor * vec4(0.5, 0.5, 0.6, 1.0);

	gl_FragColor = mix(fogColor, (matAmbientColor + diffuseColor + specularColor) * visibility, fogFactor);
}
