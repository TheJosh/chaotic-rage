#version 130

in vec2 fTexUV;
uniform sampler2D uTex;
uniform vec4 uAmbient;

void main()
{
	vec4 color;
	color.rgb = uAmbient.rgb * texture2D(uTex, fTexUV).rgb;
	color.a = 0.7f + sin(fTexUV.s * 20.0f) * 0.2f;
	gl_FragColor = color;
}
