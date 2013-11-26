#version 140

in vec3 fTexUV;
uniform samplerCube uTex;


void main()
{
	gl_FragColor = texture(uTex, fTexUV);
}
