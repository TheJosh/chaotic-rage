#version 140

const int MAX_BONES = 32;


in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexUV;
in uvec4 vBoneIDs;
in vec4 vBoneWeights;

out vec2 fTexUV;

uniform mat4 uMVP;
uniform mat4 uBones[MAX_BONES];


void main()
{
	mat4 BoneTransform;
	BoneTransform  = uBones[int(vBoneIDs[0])] * vBoneWeights[0];
	BoneTransform += uBones[int(vBoneIDs[1])] * vBoneWeights[1];
	BoneTransform += uBones[int(vBoneIDs[2])] * vBoneWeights[2];
	BoneTransform += uBones[int(vBoneIDs[3])] * vBoneWeights[3];

	gl_Position = uMVP * BoneTransform * vec4(vPosition, 1.0f);
	fTexUV = vTexUV;
}
