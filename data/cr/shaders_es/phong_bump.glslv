precision mediump float;

attribute vec3 vPosition;
attribute vec3 vNormal;
attribute vec2 vTexUV;
attribute vec3 vTangent;

varying vec2 fTexUV;
varying vec3 fLightDir[2];

uniform mat4 uMVP;
uniform mat4 uMV;
uniform mat3 uN;
uniform vec3 uLightPos[2];


void main()
{
	vec3 n = normalize(uN * vNormal);
	vec3 t = normalize(uN * vTangent);
	vec3 b = cross(n, t);
	
	vec4 vertexPos = uMV * vec4(vPosition, 1.0f);
	
	
	for (int i = 0; i < 2; i++) {
		vec3 lightDir = normalize(uLightPos[i] - vertexPos.xyz);
		
		vec3 v;
		v.x = dot(lightDir, t);
		v.y = dot(lightDir, b);
		v.z = dot(lightDir, n);
		
		fLightDir[i] = normalize(v);
	}
	
	fTexUV = vTexUV;
	gl_Position = uMVP * vec4(vPosition, 1.0f);
}
