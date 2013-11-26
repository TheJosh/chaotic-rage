precision mediump float;

varying vec2 fTexUV;
uniform sampler2D uTex;
uniform vec4 uColor;

void main(void) {
	gl_FragColor = vec4(1, 1, 1, texture2D(uTex, fTexUV).a) * uColor;
}
