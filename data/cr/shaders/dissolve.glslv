#version 120

varying vec2 TexCoord0;

void main()
{
	gl_Position = ftransform();
	TexCoord0 = gl_MultiTexCoord0.st;
}
