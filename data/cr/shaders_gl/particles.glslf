#version 140

in vec3 fColor;


void main()
{
	gl_FragColor = vec4(fColor, 1.0);
}
