precision mediump float;

varying vec2 fTexUV;
uniform sampler2D uTex;

void main()
{
	gl_FragColor = texture2D(uTex, fTexUV);
}
