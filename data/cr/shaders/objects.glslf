#version 120

varying vec2 TexCoord0;
uniform sampler2D uTex;

void main()
{
	gl_FragColor = texture2D(uTex, TexCoord0.st);
}
