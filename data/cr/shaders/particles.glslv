#version 120

varying vec3 vPosition;

uniform mat4 uMVP;


void main()
{
	gl_FrontColor = gl_Color;
	gl_Position = uMVP * vec4(vPosition, 1.0f);
}
