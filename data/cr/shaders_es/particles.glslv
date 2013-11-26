#version 140

in vec3 vPosition;
in vec3 vColor;

out vec3 fColor;

uniform mat4 uMVP;


void main()
{
	gl_Position = uMVP * vec4(vPosition, 1.0f);
	fColor = vColor;
}
