#define MAX_NUM_LIGHTS 4

in vec3 vPosition;
in vec2 vTexUV0;
in vec2 vTexUV1;
in vec3 vTangent;
in vec3 vBitangent;

out vec2 TexUV0;
out vec4 fShadowCoord;
out float fDepth;
out vec3 csNormal;
out vec3 wsPosition;
out vec3 csEyeDirection;
out vec3 csLightDirection[MAX_NUM_LIGHTS];

uniform mat4 uMVP;
uniform mat4 uM;
uniform mat3 uMN;
uniform mat4 uV;
uniform vec3 uLightPos[MAX_NUM_LIGHTS];
uniform vec4 uLightColor[MAX_NUM_LIGHTS];
uniform mat4 uDepthBiasMVP;

uniform sampler2D uNormal;

void main()
{
	vec3 normal = texture2D(uNormal, vTexUV0).rgb;

	gl_Position = uMVP * vec4(vPosition, 1.0);
	wsPosition = (uM * vec4(vPosition, 1.0)).xyz;
	
	csNormal = (mat3(uV) * uMN * normal).xyz;

	csEyeDirection = vec3(0.0, 0.0, 0.0) - (uV * uM * vec4(vPosition, 1.0)).xyz;
	for (int i = 0; i < MAX_NUM_LIGHTS; ++i) {
		vec3 csLightPos = (uV * vec4(uLightPos[i], 1.0)).xyz;
		csLightDirection[i] = csLightPos + csEyeDirection;
	}
	
	TexUV0 = vTexUV0;
	fShadowCoord = uDepthBiasMVP * vec4(vPosition, 1.0);
	fDepth = gl_Position.z / 350.0;	// TODO: Dont hard-code with far plane distance, use a uniform!
}
