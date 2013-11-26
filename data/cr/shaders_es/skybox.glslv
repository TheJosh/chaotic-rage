precision mediump float;

attribute vec3 vPosition;
varying vec3 fTexUV;
uniform mat4 uMVP;


void main()
{
	vec4 pos = uMVP * vec4(vPosition, 1.0);
	gl_Position = pos.xyww;
	fTexUV.xyz = vPosition.zyx;
}
