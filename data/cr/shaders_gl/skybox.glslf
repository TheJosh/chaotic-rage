in vec3 fTexUV;
in float fDepth;
uniform samplerCube uTex;
uniform sampler1D uDayNight;
uniform float uTimeOfDay;

const float LOG2 = 1.442695;


void main()
{
	vec4 skyColor = textureCube(uTex, fTexUV);
	vec4 todColor = texture(uDayNight, uTimeOfDay);
	
	float fogDensity = 1.5;
	float fogFactor = exp2(-fogDensity * fogDensity * fDepth * fDepth * LOG2);
	fogFactor = clamp(fogFactor, 0.0, 1.0);
	vec4 fogColor = vec4(0.4, 0.4, 0.6, 1.0);
	
	gl_FragColor = todColor;//mix(fogColor, , fogFactor);
}
