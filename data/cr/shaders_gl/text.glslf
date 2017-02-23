in vec2 fTexUV;
uniform sampler2D uTex;
uniform vec4 uColor;

void main(void) {
	gl_FragColor = vec4(1.0, 1.0, 1.0, texture2D(uTex, fTexUV).g) * uColor;
}
