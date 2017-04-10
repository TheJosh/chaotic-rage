in vec2 TexCoords;

uniform sampler2D gDiffuse;
uniform sampler2D gNormal;
uniform sampler2D gDepth;

struct Light {
	vec3 Position;
	vec3 Color;
	float Linear;
	float Quadratic;
	float Radius;
};

uniform Light lights[4];
uniform vec3 viewPos;
uniform mat4 viewInv;
uniform mat4 projInv;


vec3 worldPositionFromDepth(float depth);


void main()
{
	vec3 Diffuse = texture(gDiffuse, TexCoords).rgb;
	float Specular = 0.1;
	vec3 Normal = texture(gNormal, TexCoords).rgb;
	float Depth = texture(gDepth, TexCoords).rgb;

	vec3 FragPos = worldPositionFromDepth(Depth);
	vec3 viewDir  = normalize(viewPos - FragPos);

	vec3 lighting = Diffuse * 0.2;
	for (int i = 0; i < 4; ++i) {
		float distance = abs(length(lights[i].Position - FragPos));
		if (distance < lights[i].Radius) {
			// Diffuse
			vec3 lightDir = normalize(lights[i].Position - FragPos);
			vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;

			// Specular
			vec3 halfwayDir = normalize(lightDir + viewDir);
			float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
			vec3 specular = lights[i].Color * spec * Specular;

			// Attenuation
			float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
			diffuse *= attenuation;
			specular *= attenuation;

			lighting += diffuse + specular;
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
