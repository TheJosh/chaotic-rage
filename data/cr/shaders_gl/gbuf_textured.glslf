in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec4 gDiffuse;
layout (location = 2) out vec3 gNormal;

uniform sampler2D uTex;


void main()
{
	gPosition = FragPos;

	gNormal = normalize(Normal);

	gDiffuse = texture2D(uTex, TexCoords);
}
