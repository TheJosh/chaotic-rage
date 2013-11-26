precision mediump float;

varying vec3 fTexUV;
uniform samplerCube uTex;


void main()
{
	gl_FragColor = texture(uTex, fTexUV);
}
