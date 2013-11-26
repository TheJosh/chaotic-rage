precision mediump float;

attribute vec3 vPosition;
attribute vec3 vColor;

varying vec3 fColor;

uniform mat4 uMVP;


void main()
{
	gl_Position = uMVP * vec4(vPosition, 1.0f);
	fColor = vColor;
}
