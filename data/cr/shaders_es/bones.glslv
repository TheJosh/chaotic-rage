precision mediump float;

const int MAX_BONES = 32;


attribute vec3 vPosition;
attribute vec3 vNormal;
attribute vec2 vTexUV;
attribute vec4 vBoneIDs;
attribute vec4 vBoneWeights;

varying vec2 fTexUV;

uniform mat4 uMVP;
uniform mat4 uBones[MAX_BONES];


void main()
{
	mat4 BoneTransform;
	BoneTransform  = uBones[int(vBoneIDs[0])] * vBoneWeights[0];
	BoneTransform += uBones[int(vBoneIDs[1])] * vBoneWeights[1];
	BoneTransform += uBones[int(vBoneIDs[2])] * vBoneWeights[2];
	BoneTransform += uBones[int(vBoneIDs[3])] * vBoneWeights[3];

	gl_Position = uMVP * BoneTransform * vec4(vPosition, 1.0);
	fTexUV = vTexUV;
}
