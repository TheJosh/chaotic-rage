in vec3 fTexUV;
uniform samplerCube uTex;
uniform sampler1D uDayNight;
uniform float uTimeOfDay;

void main()
{
	vec4 skyColor = textureCube(uTex, fTexUV);
	vec4 todColor = texture(uDayNight, uTimeOfDay);
	gl_FragColor = skyColor;// * todColor; // FIXME
}
