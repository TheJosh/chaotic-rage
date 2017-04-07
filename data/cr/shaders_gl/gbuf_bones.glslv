#define MAX_BONES 32

in vec3 vPosition;
in vec2 vTexUV0;
in vec3 vNormal;
in vec4 vBoneIDs;
in vec4 vBoneWeights;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 uBones[MAX_BONES];


void main()
{
	mat4 BoneTransform;
	BoneTransform  = uBones[int(vBoneIDs[0])] * vBoneWeights[0];
	BoneTransform += uBones[int(vBoneIDs[1])] * vBoneWeights[1];
	BoneTransform += uBones[int(vBoneIDs[2])] * vBoneWeights[2];
	BoneTransform += uBones[int(vBoneIDs[3])] * vBoneWeights[3];

	vec4 worldPos = model * BoneTransform * vec4(vPosition, 1.0f);
	FragPos = worldPos.xyz;
	gl_Position = projection * view * worldPos;

	TexCoords = vTexUV0;

	mat3 normalMatrix = transpose(inverse(mat3(model)));
	Normal = normalMatrix * vNormal;
}
