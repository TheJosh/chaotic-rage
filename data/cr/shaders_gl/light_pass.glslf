in vec2 TexCoords;

uniform sampler2D gDiffuse;
uniform sampler2D gNormal;
uniform sampler2D gDepth;

struct Light {
	vec3 Position;
	vec3 Color;
};

uniform Light lights[4];
uniform vec3 viewPos;
uniform mat4 viewInv;
uniform mat4 projInv;


vec3 worldPositionFromDepth(float depth);


void main()
{
	vec3 Diffuse = texture(gDiffuse, TexCoords).rgb;
	vec3 Normal = texture(gNormal, TexCoords).rgb;
	float Depth = texture(gDepth, TexCoords).rgb;

	vec3 FragPos = worldPositionFromDepth(Depth);

	vec3 lighting = Diffuse * 0.1;

	for (int i = 0; i < 4; ++i) {
		float distance = abs(length(FragPos - lights[i].Position));
		if (distance < 5.0f) {
			lighting += lights[i].Color * vec3(0.1, 0.1, 0.1);
		}
	}

	gl_FragColor = vec4(lighting, 1.0);
}


vec3 worldPositionFromDepth(float depth)
{
	vec2 screenCoord = gl_FragCoord.xy / textureSize(gDiffuse, 0);
	vec4 position = vec4(screenCoord.xy, depth, 1.0);

	position = viewInv * projInv * 2.0 * (position - vec4(0.5));

	return position.xyz / position.w;
}
