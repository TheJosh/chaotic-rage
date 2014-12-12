#define MAX_NUM_LIGHTS 4
#define MAX_BONES 32

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexUV;
in vec4 vBoneIDs;
in vec4 vBoneWeights;

out vec2 TexUV;
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
uniform mat4 uBones[MAX_BONES];


void main()
{
	mat4 BoneTransform;
	BoneTransform  = uBones[int(vBoneIDs[0])] * vBoneWeights[0];
	BoneTransform += uBones[int(vBoneIDs[1])] * vBoneWeights[1];
	BoneTransform += uBones[int(vBoneIDs[2])] * vBoneWeights[2];
	BoneTransform += uBones[int(vBoneIDs[3])] * vBoneWeights[3];

	gl_Position = uMVP * BoneTransform * vec4(vPosition, 1.0);
	
	wsPosition = (uM * BoneTransform * vec4(vPosition, 1.0)).xyz;
	
	csEyeDirection = vec3(0.0, 0.0, 0.0) - (uV * uM * BoneTransform * vec4(vPosition, 1.0)).xyz;
	
	for (int i = 0; i < MAX_NUM_LIGHTS; ++i) {
		vec3 csLightPos = (uV * vec4(uLightPos[i], 1.0)).xyz;
		csLightDirection[i] = csLightPos + csEyeDirection;
	}
	
	csNormal = (mat3(uV) * uMN * vNormal).xyz;
	
	TexUV = vTexUV;
	fShadowCoord = uDepthBiasMVP * BoneTransform * vec4(vPosition, 1.0);
	fDepth = gl_Position.z / 350.0;	// TODO: Dont hard-code with far plane distance, use a uniform!
}
