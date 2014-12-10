in vec2 fTexUV;
uniform sampler2D uTex;
uniform vec4 uAmbient;

void main()
{
	gl_FragColor = uAmbient * texture2D(uTex, fTexUV);
}
