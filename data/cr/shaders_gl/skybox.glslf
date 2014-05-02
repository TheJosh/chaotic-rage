#version 140

in vec3 fTexUV;
in float fDepth;
uniform samplerCube uTex;

const float LOG2 = 1.442695;


void main()
{
	float fogDensity = 1.5f;
	float fogFactor = exp2(-fogDensity * fogDensity * fDepth * fDepth * LOG2);
	fogFactor = clamp(fogFactor, 0.0, 1.0);
	vec4 fogColor = vec4(0.4, 0.4, 0.6, 1.0);
	
	gl_FragColor = mix(fogColor, texture(uTex, fTexUV), fogFactor);
}
