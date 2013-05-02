#version 140

in vec2 fTexUV;
in vec4 fBoneWeights;
uniform sampler2D uTex;

void main()
{
	gl_FragColor = vec4(fBoneWeights.r / 32.f, fBoneWeights.g / 32.f, fBoneWeights.b / 32.f, 1.0f); // + texture2D(uTex, fTexUV);
}
