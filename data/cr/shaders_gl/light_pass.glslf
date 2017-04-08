in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gDiffuse;
uniform sampler2D gNormal;

struct Light {
	vec3 Position;
	vec3 Color;
};

uniform Light lights[4];
uniform vec3 viewPos;
uniform vec3 worldSize;


void main()
{
	vec3 FragPos = texture(gPosition, TexCoords).rgb * worldSize;
	vec3 Diffuse = texture(gDiffuse, TexCoords).rgb;
	vec3 Normal = texture(gNormal, TexCoords).rgb;

	vec3 lighting = Diffuse * 0.1;
	
	for (int i = 0; i < 4; ++i) {
		float distance = abs(length(FragPos - lights[i].Position));
		if (distance < 5.0f) {
			lighting += lights[i].Color * vec3(0.1, 0.1, 0.1);
		}
	}

	gl_FragColor = vec4(lighting, 1.0);
}
