precision mediump float;

attribute vec4 vCoord;
varying vec2 fTexUV;
uniform mat4 uMVP;

void main(void) {
	gl_Position = uMVP * vec4(vCoord.xy, 0.0, 1.0);
	fTexUV = vCoord.zw;
}
