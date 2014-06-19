#version 130

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexUV;

out vec2 TexUV;
out vec3 csNormal;
out vec3 csEyeDirection;
out vec3 csLightDirection;

uniform mat4 uMVP;
uniform mat4 uMV;
uniform mat4 uV;
uniform vec3 uLightPos[2];


void main()
{
	gl_Position = uMVP * vec4(vPosition, 1.0f);

	vec3 csPosition = (uMV * vec4(vPosition, 1.0f)).xyz;
	csEyeDirection = vec3(0.0, 0.0, 0.0) - csPosition;

	vec3 csLightPos = (uV * vec4(uLightPos[0], 1.0f)).xyz;
	csLightDirection = csLightPos + csEyeDirection;

	csNormal = (uMV * vec4(vNormal, 1.0f)).xyz;

	TexUV = vTexUV;
}
