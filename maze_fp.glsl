#version 400

// Attributes passed from the geometry shader
in vec2 uv;
in vec3 normal;

in float dist;

// Uniform (global) buffer
uniform sampler2D texture_map;
uniform float timer;

uniform vec3 fogColor;
uniform float fogDensity;
uniform float fogFactor;

const vec3 light = vec3(2.0, 1.5, 2.0);

void main() {
	// Retrieve texture value
	vec4 pixel = texture(texture_map, uv);

	float diffuse = 0.6 * max(0.0, dot(normalize(normal), normalize(light)));
	float amb = 0.4;

	gl_FragColor = mix(vec4(fogColor, 0.0), pixel * diffuse + pixel * amb, clamp(exp(-pow((dist * fogDensity), fogFactor)), 0.0, 1.0));
}