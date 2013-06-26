#version 140

in vec3 vPosition;
out vec3 fTexUV;
uniform mat4 uMVP;


void main()
{
	vec4 pos = uMVP * vec4(vPosition, 1.0f);
	gl_Position = pos.xyww;
	fTexUV.xyz = vPosition.zyx;
}
