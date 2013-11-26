precision mediump float;

attribute vec3 vPosition;
attribute vec3 vNormal;
attribute vec2 vTexUV;

varying vec2 fTexUV;

uniform mat4 uMVP;


void main()
{
	gl_Position = uMVP * vec4(vPosition, 1.0);
	fTexUV = vTexUV;
}
