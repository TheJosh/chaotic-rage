#version 130

in vec2 TexUV;
in vec4 fShadowCoord;
in float fDepth;
in vec3 csNormal;
in vec3 wsPosition;
in vec3 csEyeDirection;
in vec3 csLightDirection[10];

uniform mat4 uMVP;
uniform mat4 uMV;
uniform mat3 uN;
uniform sampler2D uTex;
uniform sampler2D uShadowMap;
uniform vec3 uLightPos[10];
uniform vec4 uLightColor[10];
uniform vec4 uAmbient;

const float LOG2 = 1.442695;

void main()
{
	float LightPower = 30.0f;
	
	vec3 n = normalize(csNormal);
	vec3 e = normalize(csEyeDirection);

	// Basic material
	vec4 matDiffuseColor = texture2D(uTex, TexUV);
	vec4 matAmbientColor = uAmbient * matDiffuseColor;
	vec4 matSpecularColor = vec4(0.3, 0.3, 0.3, 1.0);

	vec4 diffuseColor = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 specularColor = vec4(0.0, 0.0, 0.0, 0.0);
	
	// Iterate lights and add color for each light
	for (int i = 0; i < 10; ++i) {
		vec3 l = normalize(csLightDirection[i]);
		float dist = length(uLightPos[i] - wsPosition);
		
		// Diffuse
		float NdotL = clamp(dot(n, l), 0.0, 1.0);
		diffuseColor += uLightColor[i] * LightPower * NdotL / (dist*dist);

		// Specular
		vec3 r = reflect(-l, n);
		float EdotR = clamp(dot(e, r), 0.0, 1.0);
		specularColor += uLightColor[i] * LightPower * pow(EdotR, 5) / (dist*dist);
	}
	
	diffuseColor = matDiffuseColor * diffuseColor;
	specularColor = matSpecularColor * specularColor;
	
	
	// Shadow
	float shadowBias = 0.0005;
	float visibility = 1.0;
	if (texture(uShadowMap, fShadowCoord.xy).r < fShadowCoord.z - shadowBias) {
		visibility = 0.5;
	}

	// Fog
	float fogDensity = 1.5f;
	float fogFactor = exp2(-fogDensity * fogDensity * fDepth * fDepth * LOG2);
	fogFactor = clamp(fogFactor, 0.0, 1.0);
	vec4 fogColor = vec4(0.5, 0.5, 0.6, 1.0);

	gl_FragColor = mix(fogColor, (matAmbientColor + diffuseColor + specularColor) * visibility, fogFactor);
}
