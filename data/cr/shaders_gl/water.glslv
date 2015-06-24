in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexUV0;

out vec2 fTexUV;

uniform mat4 uMVP;


void main()
{
	gl_Position = uMVP * vec4(vPosition, 1.0);
	fTexUV = vTexUV0;
}
