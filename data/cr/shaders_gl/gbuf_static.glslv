in vec3 vPosition;
in vec2 vTexUV0;
in vec3 vNormal;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec4 worldPos = model * vec4(vPosition, 1.0f);
	FragPos = worldPos.xyz;
	gl_Position = projection * view * worldPos;

	TexCoords = vTexUV0;

	mat3 normalMatrix = transpose(inverse(mat3(model)));
	Normal = normalMatrix * vNormal;
}
