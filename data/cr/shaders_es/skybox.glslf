precision mediump float;

varying vec3 fTexUV;
uniform samplerCube uTex;


void main()
{
	gl_FragColor = textureCube(uTex, fTexUV);
}
