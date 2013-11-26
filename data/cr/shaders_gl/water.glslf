#version 140

in vec2 fTexUV;
uniform sampler2D uTex;

void main()
{
	gl_FragColor.rgb = texture2D(uTex, fTexUV).rgb;
	gl_FragColor.a = 0.7f + sin(fTexUV.s * 20.0f) * 0.2f;
}
