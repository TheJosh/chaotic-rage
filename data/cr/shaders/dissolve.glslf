#version 120

varying vec2 TexCoord0;
uniform sampler2D uTex;
uniform sampler2D uDissolve;
uniform float uDeath;

void main()
{
	if (texture2D(uDissolve, TexCoord0.st).r < uDeath)
		discard;

	gl_FragColor = texture2D(uTex, TexCoord0.st);
}
