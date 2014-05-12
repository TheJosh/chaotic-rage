#version 130

in vec2 fTexUV;
uniform sampler2D uTex;
uniform sampler2D uDissolve;
uniform float uDeath;

void main()
{
	if (texture2D(uDissolve, fTexUV).r < uDeath)
		discard;

	gl_FragColor = texture2D(uTex, fTexUV);
}
