#version 140

in vec2 fTexUV;
in vec4 fBoneWeights;
uniform sampler2D uTex;

void main()
{
	gl_FragColor = vec4(fBoneWeights.rgb, 0.5f) + texture2D(uTex, fTexUV);
}
