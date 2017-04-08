in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gDiffuse;
uniform sampler2D gNormal;

uniform vec3 worldSize;


void main()
{
	vec3 FragPos = texture(gPosition, TexCoords).rgb * worldSize;
	vec3 Diffuse = texture(gDiffuse, TexCoords).rgb;
	vec3 Normal = texture(gNormal, TexCoords).rgb;

	// Lighting calcs will come later

	gl_FragColor = vec4(Diffuse, 1.0);
}
