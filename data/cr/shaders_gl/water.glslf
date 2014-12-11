in vec2 fTexUV;
uniform sampler2D uTex;
uniform vec4 uAmbient;

void main()
{
	vec4 color;
	color.rgb = uAmbient.rgb * texture2D(uTex, fTexUV).rgb;
	color.a = 0.7 + sin(fTexUV.s * 20.0) * 0.2;
	gl_FragColor = color;
}
