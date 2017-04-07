in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gDiffuse;
uniform sampler2D gNormal;


void main()
{
    gl_FragColor = texture(gNormal, TexCoords);
}
