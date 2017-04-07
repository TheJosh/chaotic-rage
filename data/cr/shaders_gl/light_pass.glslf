in vec2 TexCoords;

uniform sampler2D gPosition;     //<-- Colour is currently going here
uniform sampler2D gDiffuse;
uniform sampler2D gNormal;


void main()
{
    gl_FragColor = texture(gPosition, TexCoords);
}
